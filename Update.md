# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

## [0.9.1] - 2026-06-06

### Fixed

- Fixed Chinese character garbling when copying data (using `MultiByteToWideChar` for UTF-8 to UTF-16 conversion)

## [0.9.0] - 2026-06-05

### Added

- Integrated Dear ImGui as official UI library, replacing native Win32 UI
- Integrated DirectX11 as IMGUI rendering backend
- Added hall name input field, separated from Yimai field
- Added health reminder for long shifts (confirmation prompt when shift exceeds 6 hours)
- Added time picker dialog

### Changed

- Full UI localization to Chinese
- Window size adjusted to 1200x900, font size adjusted to 24px
- Hall info section layout adjusted, fields arranged vertically
- Counter section layout adjusted, displayed horizontally
- Host data copy format changed to "HostName: Laxin:X Erxiao:X Jianlian:X Sanguan:X"
- Default time range changed to 00:00-03:00

### Fixed

- Chinese font display issue, loading project font file
- Host data numbers not displaying
- Number input fields accepting negative values
- Chinese character encoding issue when copying data (using Unicode)
- Crash after confirming health reminder
- Font file path issue, changed to `data/font/` directory structure

### Removed

- Original Win32 native UI code

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
