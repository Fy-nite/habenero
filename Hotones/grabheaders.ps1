param (
    [string]$SourceDir = ".",
    [string]$DestinationDir = "./headers",
    [switch]$MoveFiles
)

function Get-FullPath {
    param([string]$Path)
    try {
        $rp = Resolve-Path -Path $Path -ErrorAction Stop
        return $rp.ProviderPath
    } catch {
        return [System.IO.Path]::GetFullPath($Path)
    }
}

function Add-LongPathPrefix {
    param([string]$Path)
    if ($Path.StartsWith('\\\\?\\')) { return $Path }
    try {
        if ($Path.Length -ge 260) {
            if ($Path.StartsWith('\\')) {
                return '\\?\\UNC\\' + $Path.Substring(2)
            } else {
                return '\\?\\' + $Path
            }
        }
    } catch { }
    return $Path
}

# Normalize and validate source
$SourceDir = Get-FullPath $SourceDir
if (-not (Test-Path -Path $SourceDir)) {
    Write-Error "Source directory not found: $SourceDir"
    exit 1
}

# Ensure destination exists and normalize
if (-not (Test-Path -Path $DestinationDir)) {
    try { New-Item -ItemType Directory -Path $DestinationDir -Force | Out-Null } catch { Write-Error "Could not create destination: $DestinationDir"; exit 1 }
}
$DestinationDir = Get-FullPath $DestinationDir

# Prepare source URI (ensure trailing slash so relative URIs work)
$sourceRoot = [System.IO.Path]::GetFullPath($SourceDir).TrimEnd('\','/') + [System.IO.Path]::DirectorySeparatorChar
$sourceUri = New-Object System.Uri($sourceRoot)

# Header file extensions
$headerExtensions = @('*.h','*.hpp','*.hh','*.hxx')

Get-ChildItem -Path $SourceDir -Recurse -Include $headerExtensions -File | ForEach-Object {
    $file = $_
    try {
        $fileUri = New-Object System.Uri($file.FullName)
        $relative = [System.Uri]::UnescapeDataString($sourceUri.MakeRelativeUri($fileUri).ToString())
        $relative = $relative -replace '/','\\'
        if ($relative -like '..*') { $relative = $file.Name }
    } catch {
        $relative = $file.Name
    }

    $destPath = Join-Path $DestinationDir $relative
    $destDir = Split-Path -Path $destPath -Parent
    if ([string]::IsNullOrWhiteSpace($destDir)) { $destDir = $DestinationDir }

    if (-not (Test-Path -Path $destDir)) {
        try { New-Item -ItemType Directory -Path $destDir -Force | Out-Null } catch { Write-Warning "Could not create directory: $destDir"; return }
    }

    # Use LiteralPath and support long paths when necessary
    $srcLiteral = $file.FullName
    $dstLiteral = $destPath
    $dstWithPrefix = Add-LongPathPrefix $dstLiteral

    try {
        if ($MoveFiles) {
            Move-Item -LiteralPath $srcLiteral -Destination $dstWithPrefix -Force -ErrorAction Stop
        } else {
            Copy-Item -LiteralPath $srcLiteral -Destination $dstWithPrefix -Force -ErrorAction Stop
        }
        Write-Host "Processed: $relative"
    } catch {
        Write-Warning "Failed to process '$($file.FullName)' -> '$destPath': $($_.Exception.Message)"
    }
}

Write-Host "`nDone!"
