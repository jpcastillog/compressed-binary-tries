#!/bin/bash

# Script to measure sizes of generated files and delete them immediately
# Results saved in sizes_report.csv

# Default configuration variables
RESULTS_FILE="sizes_report.csv"
# DEFAULT_TEMP_DIR="/media/jpcastillog/Respaldo 1/temp_sizes"
DEFAULT_TEMP_DIR="/Users/jpcastillo/Documents/InvertedIndexes/temp_sizes"
USE_RAM=false
RAM_DISK_SIZE_GB=10  # RAM disk size in GB (adjustable as needed)
RUN_QUERIES=false
QUERIES_DIR="./queries"
QUERIES_RESULTS_DIR="query_results"
SELECTED_COLLECTIONS=""  # Empty means all collections
TIMESTAMP=$(date +"%Y-%m-%d %H:%M:%S")

# Global variables for cleanup
RAM_DISK_DEVICE=""
RAM_DISK_MOUNT=""

# Function to show usage
show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --use-ram              Use RAM disk for temporary files (faster, but limited by RAM)"
    echo "  --ram-size GB          Size of RAM disk in GB (default: $RAM_DISK_SIZE_GB)"
    echo "  --temp-dir DIR         Use specific directory for temporary files (default: $DEFAULT_TEMP_DIR)"
    echo "  --run-queries          Execute queries after building each trie"
    echo "  --queries-dir DIR      Directory containing query files (default: ./queries)"
    echo "  --queries-results-dir DIR  Directory to store query results (default: $QUERIES_RESULTS_DIR)"
    echo "  --collections LIST     Comma-separated list of collections to run (gov2,clueweb09,ccnews)"
    echo "                         Default: all collections"
    echo "  --help                 Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                     # Use default disk location, all collections"
    echo "  $0 --use-ram            # Use RAM disk (10GB default)"
    echo "  $0 --collections gov2   # Run only Gov2 collection"
    echo "  $0 --collections gov2,clueweb09  # Run Gov2 and ClueWeb09"
    echo "  $0 --run-queries        # Build tries and run queries"
    echo "  $0 --run-queries --collections gov2  # Run queries only on Gov2"
    echo "  $0 --run-queries --queries-dir ./queries  # Run queries with custom queries directory"
    echo "  $0 --run-queries --parallel t  # Run queries in parallel"
}

# Function to parse command line arguments
parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --use-ram)
                USE_RAM=true
                shift
                ;;
            --ram-size)
                if [[ -n "$2" && "$2" =~ ^[0-9]+$ ]]; then
                    RAM_DISK_SIZE_GB="$2"
                    shift 2
                else
                    echo "ERROR: --ram-size requires a numeric value"
                    show_usage
                    exit 1
                fi
                ;;
            --temp-dir)
                if [[ -n "$2" ]]; then
                    DEFAULT_TEMP_DIR="$2"
                    shift 2
                else
                    echo "ERROR: --temp-dir requires a directory path"
                    show_usage
                    exit 1
                fi
                ;;
            --run-queries)
                RUN_QUERIES=true
                shift
                ;;
            --parallel)
                if [[ -n "$2" ]]; then
                    PARALLEL="$2"
                    shift 2
                else
                    echo "ERROR: --parallel requires a boolean value (t/f)"
                    show_usage
                    exit 1
                fi
                ;;
            --queries-dir)
                if [[ -n "$2" ]]; then
                    QUERIES_DIR="$2"
                    shift 2
                else
                    echo "ERROR: --queries-dir requires a directory path"
                    show_usage
                    exit 1
                fi
                ;;
            --queries-results-dir)
                if [[ -n "$2" ]]; then
                    QUERIES_RESULTS_DIR="$2"
                    shift 2
                else
                    echo "ERROR: --queries-results-dir requires a directory path"
                    show_usage
                    exit 1
                fi
                ;;
            --collections)
                if [[ -n "$2" ]]; then
                    SELECTED_COLLECTIONS="$2"
                    shift 2
                else
                    echo "ERROR: --collections requires a comma-separated list"
                    show_usage
                    exit 1
                fi
                ;;
            --help|-h)
                show_usage
                exit 0
                ;;
            *)
                echo "ERROR: Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
}

# Function to create RAM disk (macOS)
create_ram_disk() {
    local size_gb=$1
    local size_mb=$((size_gb * 1024))
    local size_sectors=$((size_mb * 2048))  # 512 bytes per sector
    
    echo "Creating RAM disk of ${size_gb}GB..."
    
    # Create RAM disk using hdiutil
    RAM_DISK_DEVICE=$(hdiutil attach -nomount ram://$size_sectors 2>/dev/null)
    
    if [ -z "$RAM_DISK_DEVICE" ]; then
        echo "ERROR: Failed to create RAM disk"
        return 1
    fi
    
    # Format the RAM disk as HFS+
    diskutil eraseDisk HFS+ "RAMDisk" "$RAM_DISK_DEVICE" >/dev/null 2>&1
    
    if [ $? -ne 0 ]; then
        echo "ERROR: Failed to format RAM disk"
        hdiutil detach "$RAM_DISK_DEVICE" >/dev/null 2>&1
        return 1
    fi
    
    # Get the mount point
    RAM_DISK_MOUNT="/Volumes/RAMDisk"
    
    # Wait a moment for the disk to be mounted
    sleep 1
    
    if [ ! -d "$RAM_DISK_MOUNT" ]; then
        echo "ERROR: RAM disk was not mounted correctly"
        hdiutil detach "$RAM_DISK_DEVICE" >/dev/null 2>&1
        return 1
    fi
    
    echo "RAM disk created successfully at: $RAM_DISK_MOUNT"
    return 0
}

# Function to cleanup RAM disk
cleanup_ram_disk() {
    if [ -n "$RAM_DISK_DEVICE" ]; then
        echo "Cleaning up RAM disk..."
        # Unmount and detach
        diskutil unmount "$RAM_DISK_DEVICE" >/dev/null 2>&1
        hdiutil detach "$RAM_DISK_DEVICE" >/dev/null 2>&1
        RAM_DISK_DEVICE=""
        RAM_DISK_MOUNT=""
        echo "RAM disk cleaned up"
    fi
}

# Function to setup temporary directory
setup_temp_dir() {
    if [ "$USE_RAM" = true ]; then
        # Create RAM disk
        if ! create_ram_disk "$RAM_DISK_SIZE_GB"; then
            echo "WARNING: Failed to create RAM disk, falling back to default directory"
            TEMP_DIR="${DEFAULT_TEMP_DIR}"
            USE_RAM=false
        else
            TEMP_DIR="${RAM_DISK_MOUNT}/temp_sizes"
        fi
    else
        TEMP_DIR="${DEFAULT_TEMP_DIR}"
    fi
    
    # Create temporary directory if it doesn't exist
    mkdir -p "$TEMP_DIR"
    
    if [ ! -d "$TEMP_DIR" ]; then
        echo "ERROR: Failed to create temporary directory: $TEMP_DIR"
        exit 1
    fi
    
    echo "Using temporary directory: $TEMP_DIR"
    if [ "$USE_RAM" = true ]; then
        echo "  (RAM disk - ${RAM_DISK_SIZE_GB}GB)"
    else
        echo "  (Disk storage)"
    fi
}

# Parse command line arguments
parse_arguments "$@"

# Setup temporary directory (RAM or disk)
setup_temp_dir

# Set trap to cleanup RAM disk on exit
if [ "$USE_RAM" = true ]; then
    trap cleanup_ram_disk EXIT INT TERM
fi

# Dataset paths
# GOV2_COLLECTION="/media/jpcastillog/Respaldo 1/PISA/Gov2/gov2.docs"
GOV2_COLLECTION="/Users/jpcastillo/Documents/InvertedIndexes/Gov2/gov2.docs"
CLUEWEB09_COLLECTION="/media/jpcastillog/Respaldo 1/PISA/ClueWeb09/clueweb09.docs"
CCNEWS_COLLECTION="/media/jpcastillog/Respaldo 1/PISA/CC-News/cc-news_4096.docs"

# Function to check if a collection should be included
should_include_collection() {
    local collection_path="$1"
    local collection_name=$(basename "$collection_path" | sed 's/\.[^.]*$//' | tr '[:upper:]' '[:lower:]')
    
    # If no selection specified, include all
    if [ -z "$SELECTED_COLLECTIONS" ]; then
        return 0
    fi
    
    # Check if collection name matches any in the selection
    IFS=',' read -ra COLLS <<< "$SELECTED_COLLECTIONS"
    for coll in "${COLLS[@]}"; do
        coll=$(echo "$coll" | tr '[:upper:]' '[:lower:]' | xargs)
        case "$collection_name" in
            *gov2*)
                if [[ "$coll" == "gov2" ]]; then
                    return 0
                fi
                ;;
            *clueweb09*|*clueweb*)
                if [[ "$coll" == "clueweb09" || "$coll" == "clueweb" ]]; then
                    return 0
                fi
                ;;
            *cc-news*|*ccnews*)
                if [[ "$coll" == "ccnews" || "$coll" == "cc-news" ]]; then
                    return 0
                fi
                ;;
        esac
    done
    
    return 1
}

# Executable paths
BUILD_EXEC="./build/tools/build"
BUILD_FASTTRIES_EXEC="./build/tools/build_fastTries"
QUERIES_EXEC="./build/tools/queries"
QUERIES_FASTTRIES_EXEC="./build/tools/intersection_querylog_fastTrie"

# Function to get query file path for a collection
get_query_file() {
    local collection="$1"
    local collection_name=$(basename "$collection" | sed 's/\.[^.]*$//' | tr '[:upper:]' '[:lower:]')
    
    # Default queries directory if not specified
    local qdir="${QUERIES_DIR:-./queries}"
    
    # Map collection names to query files
    case "$collection_name" in
        *gov2*)
            echo "${qdir}/Gov2/1mq.txt"
            ;;
        *clueweb09*|*clueweb*)
            echo "${qdir}/Clueweb09/1mq.txt"
            ;;
        *cc-news*|*ccnews*)
            echo "${qdir}/CC-news/queries.txt"
            ;;
        *)
            # Try to find a matching directory
            if [ -d "${qdir}/${collection_name}" ]; then
                # Look for common query file names
                for qfile in "1mq.txt" "queries.txt" "queries"; do
                    if [ -f "${qdir}/${collection_name}/${qfile}" ]; then
                        echo "${qdir}/${collection_name}/${qfile}"
                        return 0
                    fi
                done
            fi
            echo ""
            return 1
            ;;
    esac
}

# Function to execute queries on a built trie
run_queries() {
    local trie_file="$1"
    local collection="$2"
    local min_size="$3"
    local rank_type="$4"
    local block_size="$5"
    local runs="$6"
    local is_fasttries="$7"  # "true" or "false"
    local wsize="$8"  # Only for fastTries
    local parallel="$9"  # "true" or "false"
    
    local query_file=$(get_query_file "$collection")
    
    if [ -z "$query_file" ] || [ ! -f "$query_file" ]; then
        echo "  WARNING: Query file not found for collection: $collection"
        return 1
    fi
    
    # Create results directory if it doesn't exist
    mkdir -p "$QUERIES_RESULTS_DIR"
    
    # Generate output filename
    local collection_name=$(basename "$collection" | sed 's/\.[^.]*$//')
    local rank_str="${rank_type}"
    if [ -n "$block_size" ]; then
        rank_str="${rank_type}_${block_size}"
    fi
    if [ "$is_fasttries" = "true" ] && [ -n "$wsize" ]; then
        rank_str="${rank_type}_w${wsize}"
    fi
    
    local results_file="${QUERIES_RESULTS_DIR}/${collection_name}_min${min_size}_rank${rank_str}_runs${runs}.csv"
    
    echo "  Running queries on: $trie_file"
    echo "  Query file: $query_file"
    echo "  Results file: $results_file"
    
    if [ "$is_fasttries" = "true" ]; then
        # Run queries for fastTries
        if [ -f "$QUERIES_FASTTRIES_EXEC" ]; then
            "$QUERIES_FASTTRIES_EXEC" "$trie_file" "$query_file" --verbose --parallel "$parallel" > "$results_file" 2>&1
        else
            echo "  ERROR: Query executable not found: $QUERIES_FASTTRIES_EXEC"
            return 1
        fi
    else
        # Run queries for regular tries
        if [ -f "$QUERIES_EXEC" ]; then
            "$QUERIES_EXEC" "$trie_file" "$query_file" --out "$results_file" > /tmp/query_output.log 2>&1
        else
            echo "  ERROR: Query executable not found: $QUERIES_EXEC"
            return 1
        fi
    fi
    
    if [ $? -eq 0 ]; then
        echo "  Queries completed successfully. Results saved to: $results_file"
    else
        echo "  ERROR: Query execution failed. Check: $results_file"
        return 1
    fi
}

# Function to convert bytes to MB and GB
format_size() {
    local bytes=$1
    local mb=$(echo "scale=2; $bytes / 1024 / 1024" | bc)
    local gb=$(echo "scale=3; $bytes / 1024 / 1024 / 1024" | bc)
    echo "$mb MB ($gb GB)"
}

# Function to show progress bar with collection info
show_progress_bar() {
    local current=$1
    local total=$2
    local collection_name="${3:-}"
    local config_info="${4:-}"
    local width=40
    local percent=$((current * 100 / total))
    local filled=$((current * width / total))
    local empty=$((width - filled))
    
    # Build progress bar
    local bar=""
    local i=0
    while [ $i -lt $filled ]; do
        bar="${bar}█"
        i=$((i + 1))
    done
    while [ $i -lt $width ]; do
        bar="${bar}░"
        i=$((i + 1))
    done
    
    # Clear line and print progress
    printf "\r\033[K"  # Clear to end of line
    if [ -n "$collection_name" ] && [ -n "$config_info" ]; then
        printf "Progress: [%s] %3d%% (%d/%d) | %s | %s" "$bar" "$percent" "$current" "$total" "$collection_name" "$config_info"
    else
        printf "Progress: [%s] %3d%% (%d/%d)" "$bar" "$percent" "$current" "$total"
    fi
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
    local is_fasttries="${11:-false}"  # "true" or "false"
    local wsize="${12:-}"  # Word size for fastTries
    
    local temp_file=""
    local actual_cmd=""
    local percent=$((current * 100 / total))
    
    # Get collection name and build config info
    local collection_name=$(basename "$collection" | sed 's/\.[^.]*$//')
    local rank_str="${rank_type}"
    if [ -n "$block_size" ]; then
        rank_str="${rank_type}_${block_size}"
    fi
    if [ "$is_fasttries" = "true" ] && [ -n "$wsize" ]; then
        rank_str="${rank_type}_w${wsize}"
    fi
    local config_info="min${min_size}_${rank_str}_runs${runs}"
    
    # Show progress with collection and config info
    show_progress_bar "$current" "$total" "$collection_name" "$config_info"
    echo ""  # New line after progress bar
    
    # If no --out specified, create temporary file
    if [ -z "$out_file" ]; then
        temp_file="${TEMP_DIR}/${collection_name}_min${min_size}_rank${rank_str}_runs${runs}.bin"
        # Build command with temporary --out (use printf %q to properly escape spaces)
        actual_cmd="$full_cmd --out $(printf %q "$temp_file")"
    else
        actual_cmd="$full_cmd"
        temp_file="$out_file"
    fi
    
    # Check if input file exists before processing
    if [ ! -f "$collection" ]; then
        echo "  ⚠️  WARNING: Input file does not exist: $collection"
        echo "  Skipping this configuration..."
        echo "\"$TIMESTAMP\",\"$collection\",\"$min_size\",\"$rank_type\",\"${block_size:-N/A}\",\"$runs\",\"$temp_file\",\"ERROR: Input file not found\",\"N/A\",\"N/A\"" >> "$RESULTS_FILE"
        show_progress_bar "$current" "$total" "$collection_name" "$config_info"
        echo ""
        return
    fi
    
    echo "  → Building trie: $config_info"
    
    # Execute command and capture output
    if eval "$actual_cmd" > /tmp/build_output.log 2>&1; then
        # Check if file exists and get its size
        if [ -f "$temp_file" ]; then
            local size_bytes=$(stat -f%z "$temp_file" 2>/dev/null || stat -c%s "$temp_file" 2>/dev/null || echo "0")
            local size_mb=$(echo "scale=2; $size_bytes / 1024 / 1024" | bc)
            local size_gb=$(echo "scale=3; $size_bytes / 1024 / 1024 / 1024" | bc)
            
            # Verify file was actually generated (size > 0)
            if [ "$size_bytes" = "0" ]; then
                echo "  ⚠️  WARNING: File generated but size is 0 bytes"
                echo "  Command output:"
                cat /tmp/build_output.log
            fi
            
            # Save result
            echo "\"$TIMESTAMP\",\"$collection\",\"$min_size\",\"$rank_type\",\"${block_size:-N/A}\",\"$runs\",\"$temp_file\",\"$size_bytes\",\"$size_mb\",\"$size_gb\"" >> "$RESULTS_FILE"
            
            echo "  ✓ Size: $size_bytes bytes ($size_mb MB / $size_gb GB)"
            
            # Execute queries if requested
            if [ "$RUN_QUERIES" = true ]; then
                echo "  → Running queries..."
                run_queries "$temp_file" "$collection" "$min_size" "$rank_type" "$block_size" "$runs" "$is_fasttries" "$wsize" "$parallel"
            fi
            
            # Delete file after queries (or immediately if not running queries)
            rm -f "$temp_file"
            if [ "$RUN_QUERIES" = true ]; then
                echo "  ✓ Completed (queries executed, file deleted)"
            else
                echo "  ✓ Completed (file deleted)"
            fi
        else
            echo "  ✗ ERROR: File not generated: $temp_file"
            echo "  Command output:"
            cat /tmp/build_output.log
            echo "\"$TIMESTAMP\",\"$collection\",\"$min_size\",\"$rank_type\",\"${block_size:-N/A}\",\"$runs\",\"$temp_file\",\"ERROR: File not generated\",\"N/A\",\"N/A\"" >> "$RESULTS_FILE"
        fi
    else
        echo "  ✗ ERROR executing command"
        echo "  Command output:"
        cat /tmp/build_output.log
        echo "\"$TIMESTAMP\",\"$collection\",\"$min_size\",\"$rank_type\",\"${block_size:-N/A}\",\"$runs\",\"$temp_file\",\"ERROR executing\",\"N/A\",\"N/A\"" >> "$RESULTS_FILE"
    fi
    
    # Update progress bar at the end
    show_progress_bar "$current" "$total" "$collection_name" "$config_info"
    echo ""  # New line for next iteration
}

# Initialize results file with headers
echo "Timestamp,Collection,MinSize,Rank,BlockSize,Runs,OutputFile,Bytes,MB,GB" > "$RESULTS_FILE"

echo "Starting size measurements..."
echo "Results file: $RESULTS_FILE"

# Show selected collections
if [ -n "$SELECTED_COLLECTIONS" ]; then
    echo "Selected collections: $SELECTED_COLLECTIONS"
else
    echo "Selected collections: ALL (gov2, clueweb09, ccnews)"
fi

if [ "$RUN_QUERIES" = true ]; then
    echo "Query execution: ENABLED"
    echo "Queries directory: ${QUERIES_DIR:-./queries}"
    echo "Query results directory: $QUERIES_RESULTS_DIR"
    mkdir -p "$QUERIES_RESULTS_DIR"
fi

# Calculate total configurations based on selected collections
# This will be updated after loading configs, but we set a default
TOTAL_CONFIGS=72
CURRENT_CONFIG=0

# Count collections that will be processed
COLLECTIONS_COUNT=0
if should_include_collection "$GOV2_COLLECTION"; then
    COLLECTIONS_COUNT=$((COLLECTIONS_COUNT + 1))
fi
if should_include_collection "$CLUEWEB09_COLLECTION"; then
    COLLECTIONS_COUNT=$((COLLECTIONS_COUNT + 1))
fi
if should_include_collection "$CCNEWS_COLLECTION"; then
    COLLECTIONS_COUNT=$((COLLECTIONS_COUNT + 1))
fi

# Calculate total: 
# BUILD: per collection: 4 (v/v5 with f/t) + 4 (il blocks) = 8
# FASTTRIES: per collection: 2 ranks × 2 runs × 4 wsizes = 16
# Total per collection: 8 + 16 = 24
TOTAL_CONFIGS=$((COLLECTIONS_COUNT * 24))

echo "Collections to process: $COLLECTIONS_COUNT"
echo "Total configurations to process: $TOTAL_CONFIGS"
echo ""

# Load configurations from external file
# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIG_FILE="${SCRIPT_DIR}/space_configs.sh"

if [ ! -f "$CONFIG_FILE" ]; then
    echo "ERROR: Configuration file not found: $CONFIG_FILE"
    exit 1
fi

# Source the configuration file to load the execute_all_configurations function
source "$CONFIG_FILE"

# Execute all configurations
# The function uses the global variables defined above
execute_all_configurations

# Clean up temporary directory
if [ "$USE_RAM" = false ]; then
    # Only remove directory if not using RAM (RAM disk cleanup is handled by trap)
rm -rf "$TEMP_DIR"
fi
rm -f /tmp/build_output.log

echo ""
echo "=========================================="
echo "Measurement completed!"
echo "Results saved in: $RESULTS_FILE"
if [ "$RUN_QUERIES" = true ]; then
    echo "Query results saved in: $QUERIES_RESULTS_DIR"
    query_count=$(find "$QUERIES_RESULTS_DIR" -name "*.csv" 2>/dev/null | wc -l | tr -d ' ')
    if [ "$query_count" -gt 0 ]; then
        echo "  Total query result files: $query_count"
    fi
fi
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
