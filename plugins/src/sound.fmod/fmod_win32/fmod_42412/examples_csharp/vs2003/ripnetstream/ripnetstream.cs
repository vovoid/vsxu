/*===============================================================================================
 RipNetStream Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how to rip streaming audio from the internet to local files, using
 System::attachFileSystem.
 
 This example also uses tags from the net stream to name the files as they are ripped.

 Some internet radio station only broadcast new tag information on fixed intervals.  This 
 means that the rip might not be exactly in line with the filenames that are produced.  
 For example if a radio station only broadcast the track name every 10 seconds, and the 
 music changed half way inbetween this period, then the first file would contain 5 seconds 
 of the new song, and the second song would be missing  the first 5 seconds of the track.
 Silence detection might help this problem, but most radio stations do not offer silence 
 between  tracks.
===============================================================================================*/

using System;
using System.IO;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Runtime.InteropServices;

namespace ripnetstream
{
	public class RipNetStream : System.Windows.Forms.Form
	{
        /*
            File callbacks 
        */
        private static FMOD.FILE_OPENCALLBACK  opencallback   = new FMOD.FILE_OPENCALLBACK(myopen);
        private static FMOD.FILE_CLOSECALLBACK closecallback  = new FMOD.FILE_CLOSECALLBACK(myclose);
        private static FMOD.FILE_READCALLBACK  readcallback   = new FMOD.FILE_READCALLBACK(myread);

        private FMOD.System     system  = null;
        private FMOD.Sound      sound   = null;
        private FMOD.Channel    channel = null;
        private bool            soundcreated = false;
        private string          url;
        private FMOD.CREATESOUNDEXINFO exinfo = new FMOD.CREATESOUNDEXINFO();

        private static FMOD.SOUND_TYPE gSoundType   = FMOD.SOUND_TYPE.MPEG;
        private static bool     gUpdateFileName     = false;
        private static string   gCurrentTrackArtist = "";
        private static string   gCurrentTrackTitle  = "";
        private static string   gOutputFileName     = "output.mp3";

        private System.Windows.Forms.Label label;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.Button pauseButton;
        private System.Windows.Forms.Button playButton;
        private System.Windows.Forms.TextBox textBox;
        private System.Windows.Forms.Button muteButton;
        private System.Windows.Forms.Timer timer;
        private System.ComponentModel.IContainer components;

		public RipNetStream()
		{
			InitializeComponent();
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
                FMOD.RESULT result;

                /*
                    Shutdown 
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
            this.components = new System.ComponentModel.Container();
            this.label = new System.Windows.Forms.Label();
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.exit_button = new System.Windows.Forms.Button();
            this.pauseButton = new System.Windows.Forms.Button();
            this.playButton = new System.Windows.Forms.Button();
            this.muteButton = new System.Windows.Forms.Button();
            this.textBox = new System.Windows.Forms.TextBox();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(14, 16);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 8;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 195);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(292, 24);
            this.statusBar.TabIndex = 17;
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(110, 168);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 19;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // pauseButton
            // 
            this.pauseButton.Location = new System.Drawing.Point(152, 88);
            this.pauseButton.Name = "pauseButton";
            this.pauseButton.Size = new System.Drawing.Size(136, 32);
            this.pauseButton.TabIndex = 22;
            this.pauseButton.Text = "Pause";
            this.pauseButton.Click += new System.EventHandler(this.pauseButton_Click);
            // 
            // playButton
            // 
            this.playButton.Location = new System.Drawing.Point(6, 88);
            this.playButton.Name = "playButton";
            this.playButton.Size = new System.Drawing.Size(136, 32);
            this.playButton.TabIndex = 21;
            this.playButton.Text = "Start";
            this.playButton.Click += new System.EventHandler(this.playButton_Click);
            // 
            // muteButton
            // 
            this.muteButton.Location = new System.Drawing.Point(78, 128);
            this.muteButton.Name = "muteButton";
            this.muteButton.Size = new System.Drawing.Size(136, 32);
            this.muteButton.TabIndex = 23;
            this.muteButton.Text = "Mute";
            this.muteButton.Click += new System.EventHandler(this.muteButton_Click);
            // 
            // textBox
            // 
            this.textBox.Location = new System.Drawing.Point(8, 56);
            this.textBox.Name = "textBox";
            this.textBox.Size = new System.Drawing.Size(280, 20);
            this.textBox.TabIndex = 24;
            this.textBox.Text = "http://";
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // RipNetStream
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(292, 219);
            this.Controls.Add(this.textBox);
            this.Controls.Add(this.muteButton);
            this.Controls.Add(this.pauseButton);
            this.Controls.Add(this.playButton);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.label);
            this.Name = "RipNetStream";
            this.Text = "Rip Netstream Example";
            this.Load += new System.EventHandler(this.RipNetStream_Load);
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new RipNetStream());
		}

        private void RipNetStream_Load(object sender, System.EventArgs e)
        {
            uint        version = 0;
            FMOD.RESULT result;

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
                Bump up the file buffer size a little bit for netstreams (to account for lag).  Decode buffer is left at default.
            */
            result = system.setStreamBufferSize(128*1024, FMOD.TIMEUNIT.RAWBYTES);
            ERRCHECK(result);

            result = system.attachFileSystem(opencallback, closecallback, readcallback, null);
            ERRCHECK(result);
        }

        private void playButton_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            if (!soundcreated)
            {
                statusBar.Text = "Buffering...";

                url = textBox.Text;

                result = system.createSound(url, (FMOD.MODE.HARDWARE | FMOD.MODE._2D | FMOD.MODE.CREATESTREAM | FMOD.MODE.NONBLOCKING), ref sound);
                ERRCHECK(result);

                soundcreated = true;
            }       
        }

        private void pauseButton_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            bool        paused = false;

            if (channel != null)
            {
                result = channel.getPaused(ref paused);
                ERRCHECK(result);
                result = channel.setPaused(!paused);
                ERRCHECK(result);
            }       
        }

        private void muteButton_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            bool        mute = false;

            if (channel != null)
            {
                result = channel.getMute(ref mute);
                ERRCHECK(result);
                result = channel.setMute(!mute);
                ERRCHECK(result);
            }
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            FMOD.RESULT     result;
            FMOD.OPENSTATE  openstate = 0;

            if (soundcreated)
            {
                uint percentbuffered = 0;
                bool starving = false;

                result = sound.getOpenState(ref openstate, ref percentbuffered, ref starving);
                ERRCHECK(result);

                if (openstate == FMOD.OPENSTATE.READY && channel == null)
                {
                    result = system.playSound(FMOD.CHANNELINDEX.FREE, sound, false, ref channel);
                    ERRCHECK(result);
                }
            }

            if (channel != null)
            {
                uint ms      = 0;
                bool playing = false;
                bool paused  = false;
                int  tagsupdated = 0;
                int  numtags = 0;

                result = sound.getNumTags(ref numtags, ref tagsupdated);
                ERRCHECK(result);

                if (tagsupdated != 0)
                {
                    for (;;)
                    {
                        FMOD.TAG tag = new FMOD.TAG();

                        if (sound.getTag(null, -1, ref tag) != FMOD.RESULT.OK)
                        {
                            break;
                        }

                        if (tag.datatype == FMOD.TAGDATATYPE.STRING)
                        {
                            FMOD.SOUND_FORMAT   format = FMOD.SOUND_FORMAT.NONE;
                            int                 channels = 0;
                            int                 bits = 0;

                            sound.getFormat(ref gSoundType, ref format, ref channels, ref bits);
                        
                            if (tag.name == "ARTIST")
                            {
                                if (Marshal.PtrToStringAnsi(tag.data) != gCurrentTrackArtist)
                                {
                                    gCurrentTrackArtist = Marshal.PtrToStringAnsi(tag.data);
                                    gUpdateFileName = true;
                                }
                            }
                            if (tag.name == "TITLE")
                            {
                                if (Marshal.PtrToStringAnsi(tag.data) != gCurrentTrackTitle)
                                {
                                    gCurrentTrackTitle = Marshal.PtrToStringAnsi(tag.data);
                                    gUpdateFileName = true;
                                }
                            }
                            break;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                
                result = channel.isPlaying(ref playing);
                if (result != FMOD.RESULT.OK || !playing)
                {
                    sound.release();
                    sound = null;               
                    channel = null;
                }
                else
                {
                    result = channel.getPaused(ref paused);
                    ERRCHECK(result);
                    result = channel.getPosition(ref ms, FMOD.TIMEUNIT.MS);
                    ERRCHECK(result);

                    statusBar.Text = "Time " + (ms /1000 / 60) + ":" + (ms / 1000 % 60) + ":" + (ms / 10 % 100) + (paused ? " Paused " : playing ? " Playing" : " Stopped");
                }
            }

            if (sound != null)
            {
                uint percentbuffered = 0;
                bool starving = false;

                sound.getOpenState(ref openstate, ref percentbuffered, ref starving);

                if (openstate == FMOD.OPENSTATE.ERROR)
                {
                    sound.release();
                    sound = null;               
                    channel = null;
                }
            }

            if (openstate == FMOD.OPENSTATE.ERROR)
            {
                statusBar.Text = "Error occurred or stream ended.  Restarting stream..";
                result = system.createSound(url, (FMOD.MODE.HARDWARE | FMOD.MODE._2D | FMOD.MODE.CREATESTREAM | FMOD.MODE.NONBLOCKING), ref exinfo, ref sound);
                ERRCHECK(result);
            }

            if (system != null)
            {
                system.update();
            }
        }

        private void ERRCHECK(FMOD.RESULT result)
        {
            if (result != FMOD.RESULT.OK)
            {
                timer.Stop();
                MessageBox.Show("FMOD error! " + result + " - " + FMOD.Error.String(result));
                Environment.Exit(-1);
            }
        }

        /*
            File Callbacks 
        */
        private static FileStream fs  = null;

        private static FMOD.RESULT myopen(string name, int unicode, ref uint filelength, ref IntPtr handle, ref IntPtr userdata)
        {
            fs = new FileStream(gOutputFileName, FileMode.Create, FileAccess.Write);

            return FMOD.RESULT.OK;
        }

        private static FMOD.RESULT myclose(IntPtr handle, IntPtr userdata)
        {
            fs.Close();

            return FMOD.RESULT.OK;
        }

        private static FMOD.RESULT myread(IntPtr handle, IntPtr buffer, uint sizebytes, ref uint bytesread, IntPtr userdata)
        {
            byte[] filebuffer = new byte[(int)sizebytes];

            Marshal.Copy(buffer, filebuffer, 0, (int)sizebytes);

            fs.Write(filebuffer, 0, (int)sizebytes);
            
            /*
                If the main thread detected a new tag name for artist / title, close the file and open a new one with the new name.
            */
            if (gUpdateFileName)
            {
                string ext;

                gUpdateFileName = false;

                fs.Close();

                switch (gSoundType)
                {
                    case FMOD.SOUND_TYPE.MPEG:       /* MP2/MP3 */
                    {
                        ext = ".mp3";
                        break;
                    }
                    case FMOD.SOUND_TYPE.OGGVORBIS:  /* Ogg vorbis */
                    {
                        ext = ".ogg";
                        break;
                    }
                    default:
                    {
                        ext = ".unknown";
                        break;
                    }
                }
    
                /*
                    If it is the 'temp file name' then rename the file and append to it instead of starting a new file
                */
                if (gOutputFileName == "output.mp3")
                {
                    gOutputFileName = gCurrentTrackArtist + (gCurrentTrackTitle == "" ? "" : "-") + gCurrentTrackTitle + ext;
                    
                    File.Delete(gOutputFileName);
                    File.Move("output.mp3", gOutputFileName);

                    fs = new FileStream(gOutputFileName, FileMode.Append, FileAccess.Write);
                }
                else
                {
                    gOutputFileName = gCurrentTrackArtist + (gCurrentTrackTitle == "" ? "" : "-") + gCurrentTrackTitle + ext;

                    fs = new FileStream(gOutputFileName, FileMode.Create, FileAccess.Write);
                }
            }

            return FMOD.RESULT.OK;
        }
	}
}
