# PowerShell test script (for testing on Windows CI services)
param(
  [Parameter(Mandatory=$true, HelpMessage="Path to the Finite Galaxy executable file")]
  $FiniteGalaxy #The input binary to execute
)
# Ensure the argument is a path to the binary (lest Start-Process complain).
$FiniteGalaxy = Resolve-Path -Path "$FiniteGalaxy" -Relative;

$av = !!$env:APPVEYOR;
$testName = "Parse Game Data";
if ($av)
{
  Add-AppveyorTest -Name $testName -Framework 'ES' -FileName $FiniteGalaxy -Outcome Running;
}
# Remove any existing error files first.
$FILEDIR = if ($IsWindows -or $Env:OS.ToLower().Contains('windows')) 
             { "$env:APPDATA\finite-galaxy\" } `
           else { "$env:HOME/.local/share/finite-galaxy" };
$ERR_FILE = Join-Path -Path $FILEDIR -ChildPath "errors.txt";
if (Test-Path -Path $ERR_FILE) { Remove-Item -Path $ERR_FILE; }

# Parse the game data files
$start = $(Get-Date);
$p = Start-Process -FilePath "$FiniteGalaxy" -ArgumentList '-p' -Wait -PassThru;
$dur = New-TimeSpan -Start $start -End $(Get-Date);

# Assert there is no content in the "errors.txt" file.
if ((Test-Path -Path "$ERR_FILE") -and ((Get-Content -Path "$ERR_FILE" -Raw).Length -gt 0))
{
  $err_msg = "Assertion failed: content written to file $ERR_FILE";
  $content = Get-Content -Path "$ERR_FILE" -Raw;
  Write-Host $content;
  if ($av)
  {
    $messages = @();
    $content.Split("`n") | ForEach-Object `
    {
      if ($_.StartsWith(' ') -or $_.StartsWith('file'))
        { $messages[-1] += "`n$_"; }
      elseif ($_) { $messages += $_; }
    }
    Update-AppveyorTest -Name $testName -Framework 'ES' -FileName $FiniteGalaxy `
      -Outcome Failed -Duration $dur.TotalMilliseconds -ErrorMessage $err_msg `
      -StdErr ($messages -Join "`n`n");
  }

  Write-Error -Message $err_msg -Category ParserError;
  exit 1;
}
elseif ($av)
{
  Update-AppveyorTest -Name $testName -Framework 'ES' `
    -FileName $FiniteGalaxy -Outcome Passed -Duration $dur.TotalMilliseconds;
}
else { Write-Host "No data-parsing errors were encountered"; }
exit $p.ExitCode;

