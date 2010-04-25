#pragma once

class vsx_note {
public:
	vsx_string text; // base64-encoded, we don't care to unpack it
	vsx_vector pos;
	vsx_vector size;
	vsx_string name;
	float font_size;

	bool set(vsx_command_s* c) {
		if (c->parts.size() == 6) {
			//        0       1    2  3     4 
			//  note_create name pos size text
			name = c->parts[1];
			pos.from_string(c->parts[2]);
			size.from_string(c->parts[3]);
			text = c->parts[4];
			font_size = s2f(c->parts[5]);
			return true;
		}
		return false;
	}

	const vsx_string serialize() {
		//        0       1    2  3     4 
		//  note_create name pos size text
		return "note_create_ok "+name+" "+pos.to_string()+" "+size.to_string()+" "+text+" "+f2s(font_size);
	}
	vsx_string serialize_state() {
		//        0       1    2  3     4 
		//  note_create name pos size text
		return "note_create "+name+" "+pos.to_string()+" "+size.to_string()+" "+text+" "+f2s(font_size);
	}
};
