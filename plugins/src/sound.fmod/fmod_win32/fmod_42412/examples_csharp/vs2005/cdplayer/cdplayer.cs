/*===============================================================================================
 Cdda Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how to play CD tracks digitally
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace cdda
{
	public class cdda : System.Windows.Forms.Form
	{
        private FMOD.System     system  = null;
        private FMOD.Sound      sound   = null;
        private FMOD.Sound      cdsound = null;
        private FMOD.Channel    channel = null;
        private int             currenttrack = 0;
        private int             numtracks    = 0;
        private string[]        drives;

        private System.Windows.Forms.Label label;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.Button nextTrack;
        private System.Windows.Forms.Button pause;
        private System.Windows.Forms.Button play;
        private System.Windows.Forms.ComboBox comboBox;
        private System.Windows.Forms.Timer timer;
        private System.ComponentModel.IContainer components;

		public cdda()
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
                if (cdsound != null)
                {
                    result = cdsound.release();
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
            this.nextTrack = new System.Windows.Forms.Button();
            this.pause = new System.Windows.Forms.Button();
            this.play = new System.Windows.Forms.Button();
            this.comboBox = new System.Windows.Forms.ComboBox();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(0, 4);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 8;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 139);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(272, 24);
            this.statusBar.TabIndex = 7;
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(96, 112);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(80, 24);
            this.exit_button.TabIndex = 6;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // nextTrack
            // 
            this.nextTrack.Location = new System.Drawing.Point(96, 72);
            this.nextTrack.Name = "nextTrack";
            this.nextTrack.Size = new System.Drawing.Size(80, 32);
            this.nextTrack.TabIndex = 9;
            this.nextTrack.Text = "Next track";
            this.nextTrack.Click += new System.EventHandler(this.nextTrack_Click);
            // 
            // pause
            // 
            this.pause.Location = new System.Drawing.Point(184, 72);
            this.pause.Name = "pause";
            this.pause.Size = new System.Drawing.Size(80, 32);
            this.pause.TabIndex = 10;
            this.pause.Text = "Pause / Resume";
            this.pause.Click += new System.EventHandler(this.pause_Click);
            // 
            // play
            // 
            this.play.Location = new System.Drawing.Point(8, 72);
            this.play.Name = "play";
            this.play.Size = new System.Drawing.Size(80, 32);
            this.play.TabIndex = 11;
            this.play.Text = "Play";
            this.play.Click += new System.EventHandler(this.play_Click);
            // 
            // comboBox
            // 
            this.comboBox.Location = new System.Drawing.Point(72, 40);
            this.comboBox.Name = "comboBox";
            this.comboBox.Size = new System.Drawing.Size(136, 21);
            this.comboBox.TabIndex = 12;
            this.comboBox.Text = "Select CDROM Drive";
            this.comboBox.SelectedIndexChanged += new System.EventHandler(this.comboBox_SelectedIndexChanged);
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // cdda
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(272, 163);
            this.Controls.Add(this.comboBox);
            this.Controls.Add(this.play);
            this.Controls.Add(this.pause);
            this.Controls.Add(this.nextTrack);
            this.Controls.Add(this.label);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.exit_button);
            this.Name = "cdda";
            this.Text = "CD Player Example";
            this.Load += new System.EventHandler(this.cdda_Load);
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new cdda());
		}

        private void cdda_Load(object sender, System.EventArgs e)
        {
            uint            version = 0;
            FMOD.RESULT     result;

            play.Enabled      = false;
            pause.Enabled     = false;
            nextTrack.Enabled = false;

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
            
            result = system.init(1, FMOD.INITFLAG.NORMAL, (IntPtr)null);
            ERRCHECK(result);
            
            /*
                Bump up the file buffer size a bit from the 16k default for CDDA, because it is a slower medium.
            */
            result = system.setStreamBufferSize(64*1024, FMOD.TIMEUNIT.RAWBYTES);
            ERRCHECK(result);

            /*
                Get all drive letter and put in combo box
            */
            drives = Environment.GetLogicalDrives();

            for (int count = 0; count < drives.Length; count++)
            {
                comboBox.Items.Add(drives[count].Remove(2,1));
            }
        }

        private void play_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            
            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound, false, ref channel);
            ERRCHECK(result);

            play.Enabled = false;
        }

        private void nextTrack_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            currenttrack++;
            if (currenttrack >= numtracks)
            {
                currenttrack = 0;
            }
            result = cdsound.getSubSound(currenttrack, ref sound);
            ERRCHECK(result);
            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound, false, ref channel);
            ERRCHECK(result);
        }

        private void pause_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            bool paused = false;

            result = channel.getPaused(ref paused);
            ERRCHECK(result);
            result = channel.setPaused(!paused);
            ERRCHECK(result);
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        private void comboBox_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            string selected = drives[comboBox.SelectedIndex].Remove(2,1);

            result = system.createSound(selected, (FMOD.MODE.HARDWARE | FMOD.MODE._2D | FMOD.MODE.CREATESTREAM | FMOD.MODE.OPENONLY), ref cdsound);
            ERRCHECK(result);
            result = cdsound.getNumSubSounds(ref numtracks);
            ERRCHECK(result);
            result = cdsound.getSubSound(currenttrack, ref sound);
            ERRCHECK(result);

            play.Enabled      = true;
            pause.Enabled     = true;
            nextTrack.Enabled = true;
            comboBox.Enabled  = false;
        }
        
        private void timer_Tick(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            uint ms      = 0;
            uint lenms   = 0;
            uint lenmscd = 0;
            bool playing = false;
            bool paused  = false;

            if (channel != null)
            {   
                result = channel.getPaused(ref paused);
                ERRCHECK(result);
                result = channel.isPlaying(ref playing);
                ERRCHECK(result);
                result = channel.getPosition(ref ms, FMOD.TIMEUNIT.MS);
                ERRCHECK(result);
                result = sound.getLength(ref lenms, FMOD.TIMEUNIT.MS);
                ERRCHECK(result);
            }

            if (cdsound != null)
            {
                /*
                    Get length of entire CD.  Did you know you can also play 'cdsound' and it will play the whole CD without gaps? 
                */
                result = cdsound.getLength(ref lenmscd, FMOD.TIMEUNIT.MS);
                ERRCHECK(result);
            }

            statusBar.Text = "Total CD length: " + (lenmscd / 1000 / 60) + ":" + (lenmscd / 1000 % 60) + ":" + (lenmscd / 10 % 100) +
                            " Track " + (currenttrack + 1) + "/" + numtracks + " : " + (ms / 1000 / 60) + ":" + (ms / 1000 % 60) + ":" + (ms / 10 % 100) + "/" + (lenms / 1000 / 60) + ":" + (lenms / 1000 % 60) + ":" + (lenms / 10 % 100) + (paused ? " Paused " : playing ? " Playing" : " Stopped");
            
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
	}
}
