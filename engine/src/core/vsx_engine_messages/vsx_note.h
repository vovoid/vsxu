    
    if (cmd == "note_create") {
    	static unsigned long note_counter = 0;
    	c->parts[1] = "n"+i2s(note_counter);
    	vsx_note new_note;
    	note_counter++;
    	if (new_note.set(c)) {
    		note_map[c->parts[1]] = new_note;
    		cmd_out->add_raw(new_note.serialize());
    	}
    } else
    if (cmd == "note_update") {
    	vsx_note new_note;
    	if (new_note.set(c)) {
    		note_map[c->parts[1]] = new_note;
    	}
    } else
    if (cmd == "note_delete") {
    	note_iter = note_map.find(c->parts[1]);
    	if (note_iter != note_map.end()) {
    		note_map.erase(c->parts[1]);
    		cmd_out->add_raw("note_delete_ok "+c->parts[1]);
    	}
    } else
