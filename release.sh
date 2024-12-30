#!/bin/bash

set -e

DIST_DIR="dist"
OUTPUT_FILE="clicky.hpp"
ARCHIVE_NAME="clicky.tar.gz"
REPO_NAME="auth-xyz/clicky"
TAG_NAME=${1:-"v1.0.0"}
RELEASE_NOTES=${2:-"Release notes not provided"}

mkdir -p "$DIST_DIR"

echo "// Combined clicky.hpp" > "$DIST_DIR/$OUTPUT_FILE"
echo >> "$DIST_DIR/$OUTPUT_FILE"

echo "// --- From include/clicky.hpp ---" >> "$DIST_DIR/$OUTPUT_FILE"
cat include/clicky.hpp >> "$DIST_DIR/$OUTPUT_FILE"
echo >> "$DIST_DIR/$OUTPUT_FILE"

echo "// --- From src/clicky.cpp ---" >> "$DIST_DIR/$OUTPUT_FILE"
cat src/clicky.cpp | sed 's/^/inline /' >> "$DIST_DIR/$OUTPUT_FILE"
echo >> "$DIST_DIR/$OUTPUT_FILE"

tar -czf "$ARCHIVE_NAME" -C "$DIST_DIR" "$OUTPUT_FILE"

gh release create "$TAG_NAME" "$ARCHIVE_NAME" \
  --repo "$REPO_NAME" \
  --title "$TAG_NAME" \
  --notes "$RELEASE_NOTES"

rm -r "$DIST_DIR"
rm "$ARCHIVE_NAME"

echo "Release $TAG_NAME created successfully!"

