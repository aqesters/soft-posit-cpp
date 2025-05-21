# Running GitHub Actions Workflows Locally

This guide explains how to run the GitHub Actions workflows locally using the `act` tool.

## Prerequisites

1. Install Docker for your platform: <https://docs.docker.com/get-docker/>
2. Install `act`:

   ```bash
   brew install act
   ```

## Streamlined Workflow Structure

We've simplified the workflow structure to reduce redundancy and improve maintainability:

1. **Reusable Workflow Components**:
   - `shared-setup.yml`: Sets up the build environment and handles caching
   - `shared-build.yml`: Builds the project and runs tests as needed

2. **Main Workflow Files**:
   - `ci.yml`: Main CI workflow for GitHub
   - `local-tests.yml`: Consolidated local test workflow with input parameters

## Running Local Tests

The new `local-tests.yml` workflow replaces all previous individual test workflows with a single interactive workflow:

```bash
# Just build the project
act -W .github/workflows/local-tests.yml -P ubuntu-latest=ghcr.io/catthehacker/ubuntu:act-latest --container-architecture linux/amd64 -i

# Then choose the option:
# - build: Just build without running tests
# - p8: Run Posit8 tests
# - p16: Run Posit16 tests
# - p32: Run Posit32 tests
# - quire: Run all Quire tests
# - all: Run all tests
```

Alternatively, you can specify the test type directly:

```bash
# Example: Run only Posit8 tests
act -W .github/workflows/local-tests.yml -P ubuntu-latest=ghcr.io/catthehacker/ubuntu:act-latest --container-architecture linux/amd64 --input test_type=p8
```

## Caching System

The workflows use an efficient caching system:

1. Each workflow generates a unique cache key based on the code contents (hashing source files)
2. If a matching cache is found, the build step is skipped
3. For local development, all workflows share the same cache prefix ("local-")
4. For CI, a separate cache prefix is used ("ci-")

This ensures that you only rebuild when necessary, and test runs are much faster.

## Running the Full CI Workflow

To run the complete CI workflow:

```bash
act -W .github/workflows/ci.yml -P ubuntu-latest=ghcr.io/catthehacker/ubuntu:act-latest --container-architecture linux/amd64
```

## Troubleshooting

- If you see errors about Docker not running, make sure the Docker desktop app is open and running
- For Apple Silicon (M1/M2) Macs, the `--container-architecture linux/amd64` flag is required
- If a test is taking too long, you can press Ctrl+C to cancel it
- If you want to bypass the cache and force a rebuild, use the `--no-cache` flag with `act`

## Manual Build and Test

Alternatively, you can build and test the project directly without using GitHub Actions:

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
make -j$(nproc)

# Run specific tests
./p8_tests
./p16_tests
./p32_tests
./q8_tests
./q16_tests
./q32_tests
```
