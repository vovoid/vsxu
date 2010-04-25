{$IFDEF WIN32}
{$DEFINE MSWINDOWS}
{$ENDIF}

{$IFDEF FPC}
  {$MODE DELPHI}
  {$PACKRECORDS C}
{$ENDIF}

{$IFDEF VER110}
  {$DEFINE DELPHI_5_OR_LOWER}
{$ELSE}
  {$IFDEF VER120}
    {$DEFINE DELPHI_5_OR_LOWER}
  {$ELSE}
    {$IFDEF VER130}
      {$DEFINE DELPHI_5_OR_LOWER}
    {$ENDIF}
  {$ENDIF}
{$ENDIF}

(* Force four-byte enums *)
{$Z4}

(* Disable warning for unsafe types in Delphi 7 *)
{$IFNDEF DELPHI_5_OR_LOWER}
  {$IF CompilerVersion >= 15.0}
    {$WARN UNSAFE_TYPE OFF}
  {$IFEND}
{$ENDIF}