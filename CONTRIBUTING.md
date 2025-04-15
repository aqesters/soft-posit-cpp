# Contributing to SoftPosit

Thank you for your interest in contributing to SoftPosit! This document provides guidelines for contributing to this project.

## Development Workflow

1. Fork the repository
2. Create a new branch for your feature/fix:

   ```bash
   git checkout -b feature/your-feature-name
   ```

3. Make your changes
4. Test your changes thoroughly (see Testing section below)
5. Commit your changes with clear, descriptive commit messages
6. Push your branch to your fork
7. Create a Pull Request (PR) to the main repository

## Testing

Before submitting a PR, ensure all tests and benchmarks pass:

```bash
# Create build directory and configure
mkdir build && cd build
cmake ..

# Build the library
make

# Run all tests
make check

# Run all benchmarks
make bench
```

### Test Structure

- Tests are organized by posit size (p8, p16, p32) and quire size (q8, q16, q32)
- Each test suite is in its own directory under `tests/`
- Common test utilities are in `tests/common_tests/`

### Adding New Tests

- Place new test files in the appropriate test directory
- Follow the existing test file naming convention
- Include both positive and negative test cases
- Test edge cases and error conditions

## Pull Request Guidelines

- Keep PRs focused and limited to a single feature or fix
- Provide a clear description of the changes
- Include tests for new features
- Ensure all tests pass
- Update documentation if necessary
- Follow the existing code style

## Code Style

- Follow the existing code style and formatting
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions focused and single-purpose

## Reporting Issues

When reporting issues:

- Use the issue template
- Provide detailed steps to reproduce
- Include expected vs actual behavior
- Add relevant code snippets or error messages

## Questions?

Feel free to open an issue if you have any questions about contributing.
