-- vsxu_mac_wrapper.applescript
-- vsxu_mac_wrapper

--  Created by jaw on 6/1/09.

on clicked theObject
	set vsxu_release to "vsxu_0.1.18"
	set documents_folder to POSIX path of (path to documents folder)
	if title of theObject = "Start VSXu Artiste" then
		do shell script "cd " & documents_folder & vsxu_release & "; ./vsxu_artiste.mac.sh"
	end if
	if title of theObject = "Extract VSXu into your Documents folder" then
		set bundle_path to POSIX path of (path to me)
		set source_file to bundle_path & "Contents/Resources/" & vsxu_release & ".tar.gz"
		set install_command to "tar xvfz \"" & source_file & "\" -C \"" & documents_folder & "\""
		display dialog (install_command)
		display dialog ("After hitting OK, wait for a little while, there's quite a few files to unpack... When it's done you'll get a dialog telling you so.")
		do shell script install_command
		display dialog ("OK, we're done, now try clicking \"Start VSXu Artiste\"")
	end if
end clicked