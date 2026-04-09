$src='D:\git\habenero\Hotones\src\include'
$file='D:\git\habenero\Hotones\src\include\GFX\LoadingScene.hpp'
$sourceRoot=[System.IO.Path]::GetFullPath($src).TrimEnd('\','/')
$full=[System.IO.Path]::GetFullPath($file)
Write-Host "sourceRoot: $sourceRoot"
Write-Host "full: $full"
if ($full.StartsWith($sourceRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    $relative = $full.Substring($sourceRoot.Length).TrimStart('\','/')
} else {
    $relative = $file
}
Write-Host "relative: $relative"
$dest='D:\git\habenero\Hotones\headers'
$destPath = Join-Path $dest $relative
Write-Host "destPath: $destPath"
$destDir = Split-Path -Path $destPath -Parent
Write-Host "destDir: $destDir" 
