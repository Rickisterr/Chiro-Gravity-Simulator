param (
    [string]$filename,
    [string]$build
)

$filename = "${filename}_gravity_test"

if (Test-Path "$filename.cpp") {
    switch ($build) {
        "build" {
            g++ "$filename.cpp" "C:/msys64/ucrt64/src/glad.c" -o "$filename.exe" -I "C:/msys64/ucrt64/include" -L "C:/msys64/ucrt64/lib" -lglew32 -lglfw3 -lopengl32 -lgdi32
        }
        "run" {
            Start-Process -NoNewWindow -FilePath ".\$filename.exe"
        }
        default {
            Write-Output "ValueError: Second argument should only be 'build' or 'run'"
        }
    }
} else {
    Write-Output "NameError: File does not exist (possible values of first argument are '2D' or '3D')"
}
