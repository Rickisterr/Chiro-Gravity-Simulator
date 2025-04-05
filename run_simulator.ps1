param (
    [string]$filename,
    [string]$build
)

$filename = "${filename}_gravity_test"

if (Test-Path "src/$filename.cpp") {
    switch ($build) {
        "build" {
            if (-not (Test-Path "build")) {
                mkdir "build"
            }
            g++ "src/$filename.cpp" "src/Models.cpp" "src/SpaceTimeFabric.cpp" "C:/msys64/ucrt64/src/glad.c" -o "build/$filename.exe" -I "C:/msys64/ucrt64/include" -L "C:/msys64/ucrt64/lib" -lglew32 -lglfw3 -lopengl32 -lgdi32
        }
        "run" {
            try {
                Start-Process -NoNewWindow -FilePath "build/$filename.exe"
            }
            catch {
                Write-Output "Failed to run: Try building again before running"
            }
        }
        default {
            Write-Output "ValueError: Second argument should only be 'build' or 'run'"
        }
    }
} else {
    Write-Output "NameError: File does not exist (possible values of first argument are '2D' or '3D')"
}
