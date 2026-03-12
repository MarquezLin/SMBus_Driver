$ErrorActionPreference = "Stop"

$workDir   = "C:\SMBUS_Driver\SMBus_Driver\x64\Debug\McuSmbusDrv"
$signtool  = "C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\x64\signtool.exe"
$certName  = "MyDriverTestCert"
$catFile   = Join-Path $workDir "mcusmbusdrv.cat"
$infFile   = Join-Path $workDir "McuSmbusDrv.inf"
$cerFile   = Join-Path $workDir "$certName.cer"

function Remove-CertsBySubject {
    param(
        [string]$StorePath,
        [string]$SubjectText
    )

    $certs = Get-ChildItem $StorePath -ErrorAction SilentlyContinue |
        Where-Object { $_.Subject -eq "CN=$SubjectText" }

    foreach ($c in $certs) {
        Write-Host "Removing cert from $StorePath : $($c.Thumbprint)"
        Remove-Item -Path "$StorePath\$($c.Thumbprint)" -Force
    }
}

Write-Host "=== Change directory ==="
Set-Location $workDir

if (-not (Test-Path $signtool)) {
    throw "SignTool not found: $signtool"
}

if (-not (Test-Path $catFile)) {
    throw "CAT file not found: $catFile"
}

if (-not (Test-Path $infFile)) {
    throw "INF file not found: $infFile"
}

Write-Host "=== Remove old certificates with same subject ==="
Remove-CertsBySubject -StorePath "Cert:\LocalMachine\My"               -SubjectText $certName
Remove-CertsBySubject -StorePath "Cert:\LocalMachine\Root"             -SubjectText $certName
Remove-CertsBySubject -StorePath "Cert:\LocalMachine\TrustedPublisher" -SubjectText $certName

if (Test-Path $cerFile) {
    Remove-Item $cerFile -Force
}

Write-Host "=== Create new code signing certificate ==="
$cert = New-SelfSignedCertificate `
    -Type CodeSigningCert `
    -Subject "CN=$certName" `
    -FriendlyName $certName `
    -KeyUsage DigitalSignature `
    -CertStoreLocation "Cert:\LocalMachine\My"

Write-Host "=== Check certificate ==="
$cert | Format-List Subject, Thumbprint, HasPrivateKey, EnhancedKeyUsageList

if (-not $cert.HasPrivateKey) {
    throw "Certificate has no private key. Cannot sign."
}

Write-Host "=== Export certificate ==="
Export-Certificate -Cert $cert -FilePath $cerFile | Out-Null

Write-Host "=== Import into Trusted Root ==="
Import-Certificate -FilePath $cerFile -CertStoreLocation "Cert:\LocalMachine\Root" | Out-Null

Write-Host "=== Import into Trusted Publisher ==="
Import-Certificate -FilePath $cerFile -CertStoreLocation "Cert:\LocalMachine\TrustedPublisher" | Out-Null

Write-Host "=== Sign CAT file with exact thumbprint ==="
& $signtool sign `
    /v `
    /fd SHA256 `
    /sm `
    /s My `
    /sha1 $($cert.Thumbprint) `
    $catFile

Write-Host "=== Verify CAT file ==="
& $signtool verify `
    /v `
    /kp `
    $catFile

Write-Host "=== Install driver ==="
pnputil /add-driver $infFile /install

Write-Host "=== Done ==="
Read-Host "Press Enter to exit"