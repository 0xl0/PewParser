#!/bin/bash

# Constants
readonly DIRECTORY="premake"
readonly BIN_FILE="premake5"
readonly ZIP_FILE="premake.tar.gz"
readonly URL="https://github.com/premake/premake-core/releases/download/v5.0.0-beta2/premake-5.0.0-beta2-linux.tar.gz"

# Error handling
error_exit() {
    echo "Error: $1" >&2
    exit 1
}

run_premake() {
    echo "Running premake5 with premake5.lua for gmake..."
    "$DIRECTORY/$BIN_FILE" --file=premake5.lua gmake || error_exit "Failed to run Premake5"
}

# check download tool availability
check_downloader() {
    if command -v curl >/dev/null 2>&1; then
        echo "curl"
    elif command -v wget >/dev/null 2>&1; then
        echo "wget"
    else
        error_exit "Neither curl nor wget is installed. Please install one:
        Ubuntu/Debian: sudo apt install curl
        Fedora: sudo dnf install curl
        Arch: sudo pacman -S curl"
    fi
}

download_premake() {
    local downloader
    downloader=$(check_downloader)
    echo "Downloading Premake5 using $downloader..."
    if [ "$downloader" = "curl" ]; then
        curl -L -o "$DIRECTORY/$ZIP_FILE" "$URL" || error_exit "Failed to download Premake5"
    else
        wget -O "$DIRECTORY/$ZIP_FILE" "$URL" || error_exit "Failed to download Premake5"
    fi
}

install_and_verify() {
    # Extract everything
    cd "$DIRECTORY" && tar -xzf "$ZIP_FILE" && cd .. || error_exit "Failed to extract Premake5"
    
    # Verify and set permissions
    [ -f "$DIRECTORY/$BIN_FILE" ] || error_exit "Premake5 binary not found after extraction"
    chmod +x "$DIRECTORY/$BIN_FILE" || error_exit "Failed to set executable permission"
    
    echo "Premake5 successfully installed."
    run_premake
}

main() {
    # Create directory if it doesn't exist
    mkdir -p "$DIRECTORY" || error_exit "Failed to create premake directory"

    # Check if premake5 is already installed
    if [ -f "$DIRECTORY/$BIN_FILE" ]; then
        echo "Premake5 found."
        chmod +x "$DIRECTORY/$BIN_FILE" || error_exit "Failed to set executable permission"
        run_premake
    else
        echo "Premake5 not found."
        # Check if tarball exists
        if [ -f "$DIRECTORY/$ZIP_FILE" ]; then
            echo "Found existing $ZIP_FILE, extracting it..."
            install_and_verify
        else
            echo "Initiating download and installation..."
            download_premake
            install_and_verify
        fi
        rm -f "$DIRECTORY/$ZIP_FILE" || echo "Warning: Failed to clean up zip file"
    fi
}

# Execute main
main
exit 0
