#!/bin/bash

# Script to measure sizes of generated files and delete them immediately
# Results saved in sizes_report.csv

# Configuration variables
RESULTS_FILE="sizes_report.csv"
TEMP_DIR="/media/jpcastillog/Respaldo 1/temp_sizes"
TIMESTAMP=$(date +"%Y-%m-%d %H:%M:%S")

# Dataset paths
GOV2_COLLECTION="/media/jpcastillog/Respaldo 1/PISA/Gov2/gov2.docs"
CLUEWEB09_COLLECTION="/media/jpcastillog/Respaldo 1/PISA/ClueWeb09/clueweb09.docs"
CCNEWS_COLLECTION="/media/jpcastillog/Respaldo 1/PISA/CC-News/cc-news_4096.docs"

# Executable paths
BUILD_EXEC="./build/tools/build"
BUILD_FASTTRIES_EXEC="./build/tools/build_fastTries"

# Create temporary directory if it doesn't exist
mkdir -p "$TEMP_DIR"

# Function to convert bytes to MB and GB
format_size() {
    local bytes=$1
    local mb=$(echo "scale=2; $bytes / 1024 / 1024" | bc)
    local gb=$(echo "scale=3; $bytes / 1024 / 1024 / 1024" | bc)
    echo "$mb MB ($gb GB)"
}

# Function to show progress bar
show_progress_bar() {
    local current=$1
    local total=$2
    local width=50
    local percent=$((current * 100 / total))
    local filled=$((current * width / total))
    local empty=$((width - filled))
    
    printf "\r["
    printf "%${filled}s" | tr ' ' '='
    printf "%${empty}s" | tr ' ' '-'
    printf "] %d%% (%d/%d)" "$percent" "$current" "$total"
}

# Function to execute command, get file size and delete file
run_and_measure() {
    local current="$1"
    local total="$2"
    local collection="$3"
    local min_size="$4"
    local rank_type="$5"
    local block_size="$6"
    local runs="$7"
    local out_file="$8"
    local build_cmd="$9"
    local full_cmd="${10}"
    
    local temp_file=""
    local actual_cmd=""
    local percent=$((current * 100 / total))
    
    # Show progress
    echo ""
    show_progress_bar "$current" "$total"
    echo ""
    echo "=================================================="
    echo "[$current/$total] ($percent%) Processing configuration..."
    echo "=================================================="
    
    # If no --out specified, create temporary file
    if [ -z "$out_file" ]; then
        local collection_name=$(basename "$collection" | sed 's/\.[^.]*$//')
        local rank_str="${rank_type}"
        if [ -n "$block_size" ]; then
            rank_str="${rank_type}_${block_size}"
        fi
        temp_file="${TEMP_DIR}/${collection_name}_min${min_size}_rank${rank_str}_runs${runs}.bin"
        # Build command with temporary --out (use printf %q to properly escape spaces)
        actual_cmd="$full_cmd --out $(printf %q "$temp_file")"
    else
        actual_cmd="$full_cmd"
        temp_file="$out_file"
    fi
    
    # Check if input file exists before processing
    if [ ! -f "$collection" ]; then
        echo "  WARNING: Input file does not exist: $collection"
        echo "  Skipping this configuration..."
        echo "\"$TIMESTAMP\",\"$collection\",\"$min_size\",\"$rank_type\",\"${block_size:-N/A}\",\"$runs\",\"$temp_file\",\"ERROR: Input file not found\",\"N/A\",\"N/A\"" >> "$RESULTS_FILE"
        show_progress_bar "$current" "$total"
        echo ""
        return
    fi
    
    echo "Executing: $actual_cmd"
    
    # Execute command and capture output
    if eval "$actual_cmd" > /tmp/build_output.log 2>&1; then
        # Check if file exists and get its size
        if [ -f "$temp_file" ]; then
            local size_bytes=$(stat -f%z "$temp_file" 2>/dev/null || stat -c%s "$temp_file" 2>/dev/null || echo "0")
            local size_mb=$(echo "scale=2; $size_bytes / 1024 / 1024" | bc)
            local size_gb=$(echo "scale=3; $size_bytes / 1024 / 1024 / 1024" | bc)
            
            # Verify file was actually generated (size > 0)
            if [ "$size_bytes" = "0" ]; then
                echo "  WARNING: File generated but size is 0 bytes"
                echo "  Command output:"
                cat /tmp/build_output.log
            fi
            
            # Save result
            echo "\"$TIMESTAMP\",\"$collection\",\"$min_size\",\"$rank_type\",\"${block_size:-N/A}\",\"$runs\",\"$temp_file\",\"$size_bytes\",\"$size_mb\",\"$size_gb\"" >> "$RESULTS_FILE"
            
            echo "  Size: $size_bytes bytes ($size_mb MB / $size_gb GB)"
            
            # Delete file immediately
            rm -f "$temp_file"
            echo "  File deleted: $temp_file"
        else
            echo "  ERROR: File not generated: $temp_file"
            echo "  Command output:"
            cat /tmp/build_output.log
            echo "\"$TIMESTAMP\",\"$collection\",\"$min_size\",\"$rank_type\",\"${block_size:-N/A}\",\"$runs\",\"$temp_file\",\"ERROR: File not generated\",\"N/A\",\"N/A\"" >> "$RESULTS_FILE"
        fi
    else
        echo "  ERROR executing command"
        echo "  Command output:"
        cat /tmp/build_output.log
        echo "\"$TIMESTAMP\",\"$collection\",\"$min_size\",\"$rank_type\",\"${block_size:-N/A}\",\"$runs\",\"$temp_file\",\"ERROR executing\",\"N/A\",\"N/A\"" >> "$RESULTS_FILE"
    fi
    show_progress_bar "$current" "$total"
    echo ""
}

# Initialize results file with headers
echo "Timestamp,Collection,MinSize,Rank,BlockSize,Runs,OutputFile,Bytes,MB,GB" > "$RESULTS_FILE"

echo "Starting size measurements..."
echo "Results file: $RESULTS_FILE"
echo "Temporary directory: $TEMP_DIR"
echo "Total configurations to process: 68"
echo ""

# Count total configurations
# 20 build configurations + 48 build_fastTries configurations = 68 total
TOTAL_CONFIGS=68
CURRENT_CONFIG=0

# Array with all configurations from the original script
# Format: "collection|min_size|rank|block_size|runs|out_file|build_command|full_command"
# Only configurations with min_size=4096 are included

Gov2 configurations
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "4096" "v" "" "f" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 4096 --rank v --runs f"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "4096" "v5" "" "f" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 4096 --rank v5 --runs f"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "4096" "v" "" "t" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 4096 --rank v --runs t"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "4096" "v5" "" "t" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 4096 --rank v5 --runs t"

CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "4096" "il" "64" "t" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 4096 --rank il 64 --runs t"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "4096" "il" "128" "t" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 4096 --rank il 128 --runs t"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "4096" "il" "256" "t" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 4096 --rank il 256 --runs t"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "4096" "il" "512" "t" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 4096 --rank il 512 --runs t"

# ClueWeb09 configurations
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "4096" "v" "" "f" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 4096 --rank v --runs f"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "4096" "v5" "" "f" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 4096 --rank v5 --runs f"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "4096" "v" "" "t" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 4096 --rank v --runs t"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "4096" "v5" "" "t" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 4096 --rank v5 --runs t"

CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "4096" "il" "64" "t" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 4096 --rank il 64 --runs t"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "4096" "il" "128" "t" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 4096 --rank il 128 --runs t"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "4096" "il" "256" "t" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 4096 --rank il 256 --runs t"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "4096" "il" "512" "t" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 4096 --rank il 512 --runs t"

# CC-News configurations
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "4096" "v" "" "f" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 4096 --rank v --runs f"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "4096" "v5" "" "f" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 4096 --rank v5 --runs f"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "4096" "v" "" "t" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 4096 --rank v --runs t"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "4096" "v5" "" "t" "" "$BUILD_EXEC" "$BUILD_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 4096 --rank v5 --runs t"

# FastTries configurations (Gov2)
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v" "64" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v --runs f --wsize 64"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v" "64" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v --runs t --wsize 64"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v5" "64" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v5 --runs f --wsize 64"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v5" "64" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v5 --runs t --wsize 64"

CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v" "32" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v --runs f --wsize 32"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v" "32" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v --runs t --wsize 32"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v5" "32" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v5 --runs f --wsize 32"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v5" "32" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v5 --runs t --wsize 32"

CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v" "16" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v --runs f --wsize 16"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v" "16" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v --runs t --wsize 16"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v5" "16" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v5 --runs f --wsize 16"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v5" "16" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v5 --runs t --wsize 16"

CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v" "8" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v --runs f --wsize 8"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v" "8" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v --runs t --wsize 8"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v5" "8" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v5 --runs f --wsize 8"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$GOV2_COLLECTION" "1" "v5" "8" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$GOV2_COLLECTION\\\" --min_size 1 --rank v5 --runs t --wsize 8"

# FastTries configurations (ClueWeb09)
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v" "64" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v --runs f --wsize 64"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v" "64" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v --runs t --wsize 64"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v5" "64" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v5 --runs f --wsize 64"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v5" "64" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v5 --runs t --wsize 64"

CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v" "32" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v --runs f --wsize 32"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v" "32" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v --runs t --wsize 32"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v5" "32" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v5 --runs f --wsize 32"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v5" "32" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v5 --runs t --wsize 32"

CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v" "16" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v --runs f --wsize 16"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v" "16" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v --runs t --wsize 16"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v5" "16" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v5 --runs f --wsize 16"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v5" "16" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v5 --runs t --wsize 16"

CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v" "8" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v --runs f --wsize 8"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v" "8" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v --runs t --wsize 8"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v5" "8" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v5 --runs f --wsize 8"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CLUEWEB09_COLLECTION" "1" "v5" "8" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CLUEWEB09_COLLECTION\\\" --min_size 1 --rank v5 --runs t --wsize 8"

# FastTries configurations (CC-News)
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v" "64" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v --runs f --wsize 64"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v" "64" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v --runs t --wsize 64"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v5" "64" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v5 --runs f --wsize 64"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v5" "64" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v5 --runs t --wsize 64"

CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v" "32" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v --runs f --wsize 32"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v" "32" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v --runs t --wsize 32"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v5" "32" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v5 --runs f --wsize 32"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v5" "32" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v5 --runs t --wsize 32"

CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v" "16" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v --runs f --wsize 16"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v" "16" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v --runs t --wsize 16"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v5" "16" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v5 --runs f --wsize 16"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v5" "16" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v5 --runs t --wsize 16"

CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v" "8" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v --runs f --wsize 8"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v" "8" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v --runs t --wsize 8"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v5" "8" "f" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v5 --runs f --wsize 8"
CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$CCNEWS_COLLECTION" "1" "v5" "8" "t" "" "$BUILD_FASTTRIES_EXEC" "$BUILD_FASTTRIES_EXEC \\\"$CCNEWS_COLLECTION\\\" --min_size 1 --rank v5 --runs t --wsize 8"

# Clean up temporary directory
rm -rf "$TEMP_DIR"
rm -f /tmp/build_output.log

echo ""
echo "=========================================="
echo "Measurement completed!"
echo "Results saved in: $RESULTS_FILE"
echo "=========================================="

# Show summary
if command -v bc &> /dev/null && [ -f "$RESULTS_FILE" ]; then
    total_bytes=$(awk -F',' 'NR>1 && $8 ~ /^[0-9]+$/ {sum+=$8} END {print sum+0}' "$RESULTS_FILE" 2>/dev/null || echo "0")
    if [ "$total_bytes" != "0" ] && [ -n "$total_bytes" ]; then
        total_mb=$(echo "scale=2; $total_bytes / 1024 / 1024" | bc 2>/dev/null || echo "0")
        total_gb=$(echo "scale=3; $total_bytes / 1024 / 1024 / 1024" | bc 2>/dev/null || echo "0")
        echo ""
        echo "Total size of all variants:"
        echo "  $total_bytes bytes"
        echo "  $total_mb MB"
        echo "  $total_gb GB"
    else
        echo ""
        echo "No valid size measurements were recorded."
    fi
fi
