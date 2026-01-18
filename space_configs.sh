#!/bin/bash

# Configuration file for space.sh
# Defines all configurations that will be executed to measure sizes

# Helper function to execute a BUILD configuration
# Parameters: collection, min_size, rank, block_size, runs
run_build_config() {
    local collection="$1"
    local min_size="$2"
    local rank="$3"
    local block_size="$4"
    local runs="$5"
    
    CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
    
    # Build the command depending on whether it has block_size or not
    # Use printf %q to properly escape the collection path
    local escaped_collection=$(printf %q "$collection")
    if [ -n "$block_size" ]; then
        local full_cmd="$BUILD_EXEC $escaped_collection --min_size $min_size --rank $rank $block_size --runs $runs"
        local block_size_param="$block_size"
    else
        local full_cmd="$BUILD_EXEC $escaped_collection --min_size $min_size --rank $rank --runs $runs"
        local block_size_param=""
    fi
    
    run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$collection" "$min_size" "$rank" "$block_size_param" "$runs" "" "$BUILD_EXEC" "$full_cmd" "false" ""
}

# Helper function to execute a FASTTRIES configuration
# Parameters: collection, min_size, rank, wsize, runs
run_fasttries_config() {
    local collection="$1"
    local min_size="$2"
    local rank="$3"
    local wsize="$4"
    local runs="$5"
    local parallel="$6"
    
    CURRENT_CONFIG=$((CURRENT_CONFIG + 1))
    # Use printf %q to properly escape the collection path
    local escaped_collection=$(printf %q "$collection")
    local full_cmd="$BUILD_FASTTRIES_EXEC $escaped_collection --min_size $min_size --rank $rank --runs $runs --wsize $wsize --parallel $parallel"
    
    run_and_measure "$CURRENT_CONFIG" "$TOTAL_CONFIGS" "$collection" "$min_size" "$rank" "" "$runs" "" "$BUILD_FASTTRIES_EXEC" "$full_cmd" "true" "$wsize" "$parallel"  
}

# Function that executes all configurations
# Uses global variables from the main script (CURRENT_CONFIG, TOTAL_CONFIGS, etc.)
execute_all_configurations() {
    
    # ============================================================
    # Configuration variables - Easy to modify
    # ============================================================
    
    # Minimum size for BUILD configurations
    local BUILD_MIN_SIZE="4096"
    
    # Minimum size for FASTTRIES configurations
    local FASTTRIES_MIN_SIZE="1"
    
    # ============================================================
    # BUILD configurations
    # ============================================================
    
    # Configuration arrays - filter collections based on selection
    local build_collections_all=("$GOV2_COLLECTION" "$CLUEWEB09_COLLECTION" "$CCNEWS_COLLECTION")
    local build_collections_filtered=()
    
    # Filter collections based on SELECTED_COLLECTIONS
    for collection in "${build_collections_all[@]}"; do
        if should_include_collection "$collection"; then
            build_collections_filtered+=("$collection")
        fi
    done
    
    local build_ranks_simple=("v" "v5")
    local build_runs=("f" "t")
    local build_il_blocks=("64" "128" "256" "512")
    
    # For each collection: configurations with rank "v" and "v5" (without block_size)
    for collection in "${build_collections_filtered[@]}"; do
        for rank in "${build_ranks_simple[@]}"; do
            for runs in "${build_runs[@]}"; do
                run_build_config "$collection" "$BUILD_MIN_SIZE" "$rank" "" "$runs"
            done
        done
    done
    
    # Configurations with rank "il" for selected collections
    for collection in "${build_collections_filtered[@]}"; do
        for block_size in "${build_il_blocks[@]}"; do
            run_build_config "$collection" "$BUILD_MIN_SIZE" "il" "$block_size" "t"
        done
    done
    
    # ============================================================
    # FASTTRIES configurations
    # ============================================================
    
    local fasttries_collections_all=("$GOV2_COLLECTION" "$CLUEWEB09_COLLECTION" "$CCNEWS_COLLECTION")
    local fasttries_collections_filtered=()
    
    # Filter collections based on SELECTED_COLLECTIONS
    for collection in "${fasttries_collections_all[@]}"; do
        if should_include_collection "$collection"; then
            fasttries_collections_filtered+=("$collection")
        fi
    done
    
    local fasttries_ranks=("v" "v5")
    local fasttries_runs=("f" "t")
    local fasttries_wsizes=("64" "32" "16" "8")
    
    # For each collection
    for collection in "${fasttries_collections_filtered[@]}"; do
        # For each combination of rank, runs and wsize
        for rank in "${fasttries_ranks[@]}"; do
            for runs in "${fasttries_runs[@]}"; do
                for wsize in "${fasttries_wsizes[@]}"; do
                    run_fasttries_config "$collection" "$FASTTRIES_MIN_SIZE" "$rank" "$wsize" "$runs"
                done
            done
        done
    done
}
