/*===============================================================================================
 ReadTags Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004.

 This example shows how to read tags from sound files
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Runtime.InteropServices;

namespace readtags
{
	public class ReadTags : System.Windows.Forms.Form
	{
        private FMOD.System system = null;
        private FMOD.Sound  sound  = null;

        private System.Windows.Forms.Label label;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.ListBox listBox;

		private System.ComponentModel.Container components = null;

		public ReadTags()
		{
			InitializeComponent();
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
                FMOD.RESULT     result;
            
                /*
                    Shut down
                */
                if (sound != null)
                {
                    result = sound.release();
                    ERRCHECK(result);
                }
                if (system != null)
                {
                    result = system.close();
                    ERRCHECK(result);
                    result = system.release();
                    ERRCHECK(result);
                }

				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.label = new System.Windows.Forms.Label();
            this.exit_button = new System.Windows.Forms.Button();
            this.listBox = new System.Windows.Forms.ListBox();
            this.SuspendLayout();
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(56, 16);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 7;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(160, 240);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 14;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // listBox
            // 
            this.listBox.Location = new System.Drawing.Point(8, 56);
            this.listBox.Name = "listBox";
            this.listBox.Size = new System.Drawing.Size(360, 173);
            this.listBox.TabIndex = 15;
            // 
            // ReadTags
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(384, 271);
            this.Controls.Add(this.listBox);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.label);
            this.Name = "ReadTags";
            this.Text = "Read Tags Example";
            this.Load += new System.EventHandler(this.ReadTags_Load);
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new ReadTags());
		}

        private void ReadTags_Load(object sender, System.EventArgs e)
        {
            FMOD.TAG        tag = new FMOD.TAG();
            int             numtags = 0, numtagsupdated = 0;
            uint            version = 0;
            FMOD.RESULT     result;

            /*
                Create a System object and initialize.
            */
            result = FMOD.Factory.System_Create(ref system);
            ERRCHECK(result);

            result = system.getVersion(ref version);
            ERRCHECK(result);
            if (version < FMOD.VERSION.number)
            {
                MessageBox.Show("Error!  You are using an old version of FMOD " + version.ToString("X") + ".  This program requires " + FMOD.VERSION.number.ToString("X") + ".");
                Application.Exit();
            }
            result = system.init(100, FMOD.INITFLAG.NORMAL, (IntPtr)null);
            ERRCHECK(result);

            /*
                Open the specified file. Use FMOD_CREATESTREAM and FMOD_DONTPREBUFFER so it opens quickly
            */
            result = system.createSound("../../../../examples/media/wave.mp3", (FMOD.MODE.SOFTWARE | FMOD.MODE._2D | FMOD.MODE.CREATESTREAM | FMOD.MODE.OPENONLY), ref sound);
            ERRCHECK(result);

            /*
                Read and display all tags associated with this file
            */
            for (;;)
            {
                /*
                    An index of -1 means "get the first tag that's new or updated".
                    If no tags are new or updated then getTag will return FMOD_ERR_TAGNOTFOUND.
                    This is the first time we've read any tags so they'll all be new but after we've read them, 
                    they won't be new any more.
                */
                if (sound.getTag(null, -1, ref tag) != FMOD.RESULT.OK)
                {
                    break;
                }
                if (tag.datatype == FMOD.TAGDATATYPE.STRING)
                {
                    listBox.Items.Add(tag.name + " = " + Marshal.PtrToStringAnsi(tag.data) + " (" + tag.datalen + " bytes)");
                }
                else
                {
                    listBox.Items.Add(tag.name + " = <binary> (" + tag.datalen + " bytes)");
                }
            }

            listBox.Items.Add(" ");

            /*
                Read all the tags regardless of whether they're updated or not. Also show the tag type.
            */
            result = sound.getNumTags(ref numtags, ref numtagsupdated);
            ERRCHECK(result);
            for (int count=0; count < numtags; count++)
            {
                string tagtext = null;

                result = sound.getTag(null, count, ref tag);
                ERRCHECK(result);

                switch (tag.type)
                {
                    case FMOD.TAGTYPE.UNKNOWN :
                        tagtext = "FMOD_TAGTYPE_UNKNOWN  ";
                        break;

                    case FMOD.TAGTYPE.ID3V1 :
                        tagtext = "FMOD_TAGTYPE_ID3V1  ";
                        break;

                    case FMOD.TAGTYPE.ID3V2 :
                        tagtext = "FMOD_TAGTYPE_ID3V2  ";
                        break;

                    case FMOD.TAGTYPE.VORBISCOMMENT :
                        tagtext = "FMOD_TAGTYPE_VORBISCOMMENT  ";
                        break;

                    case FMOD.TAGTYPE.SHOUTCAST :
                        tagtext = "FMOD_TAGTYPE_SHOUTCAST  ";
                        break;

                    case FMOD.TAGTYPE.ICECAST :
                        tagtext = "FMOD_TAGTYPE_ICECAST  ";
                        break;

                    case FMOD.TAGTYPE.ASF :
                        tagtext = "FMOD_TAGTYPE_ASF  ";
                        break;

                    case FMOD.TAGTYPE.FMOD :
                        tagtext = "FMOD_TAGTYPE_FMOD  ";
                        break;

                    case FMOD.TAGTYPE.USER :
                        tagtext = "FMOD_TAGTYPE_USER  ";
                        break;
                }

                if (tag.datatype == FMOD.TAGDATATYPE.STRING)
                {
                    tagtext += (tag.name + " = " + Marshal.PtrToStringAnsi(tag.data) + "(" + tag.datalen + " bytes)");
                }
                else
                {
                    tagtext += (tag.name + " = ??? (" + tag.datalen + " bytes)");
                }

                listBox.Items.Add(tagtext);
            }

            listBox.Items.Add(" ");

            /*
                 Find a specific tag by name. Specify an index > 0 to get access to multiple tags of the same name.
            */
            result = sound.getTag("ARTIST", 0, ref tag);
            ERRCHECK(result);
            listBox.Items.Add(tag.name + " = " + Marshal.PtrToStringAnsi(tag.data) + " (" + tag.datalen + " bytes)");
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        private void ERRCHECK(FMOD.RESULT result)
        {
            if (result != FMOD.RESULT.OK)
            {
                MessageBox.Show("FMOD error! " + result + " - " + FMOD.Error.String(result));
                Environment.Exit(-1);
            }
        }
	}
}
