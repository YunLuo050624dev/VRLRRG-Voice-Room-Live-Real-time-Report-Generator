# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

## [0.8.0] - 2026-06-04

### Added

- Comprehensive debug logging system for test builds
- Console window support for debugging (only in test build)
- Pure GUI mode for release build (no console window)
- [INFO] tags for initialization and lifecycle events
- [TEST] tags for user operations (button clicks, data updates)
- [ERROR] tags for error conditions
- Dynamic window height adjustment when adding/removing hosts
- Time picker dialog with mouse wheel support
- Time validation to prevent negative duration
- Real-time data synchronization between host entries and summary counters

### Changed

- UI layout changed to vertical arrangement
- Input fields left-aligned for better visual consistency
- Improved window refresh mechanism to reduce flickering
- Updated version number to 0.8.0

### Fixed

- Host position display issues
- Button position dynamic adjustment
- Text display issues (line wrapping, slash position)
- Window height dynamic adjustment
- Scaling bugs
- Manual host data editing not syncing to summary counters

## [0.7.0] - 2026-06-03

### Added

- Basic data collection and parsing
- Simple report generation and export
- Windows platform executable packaging
- Host management (add/remove hosts)
- Counter increment functionality
- Target setting dialog
- Clipboard copy functionality

### Notes

First pre-release version. May contain unresolved bugs.

## License

Apache 2.0 License
