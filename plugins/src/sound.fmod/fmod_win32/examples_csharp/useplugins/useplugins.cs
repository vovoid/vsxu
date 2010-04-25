/*===============================================================================================
 Use Plugins Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004.

 This example shows how to use FMODEXP.DLL and its associated plugins.
 The example lists the available plugins, and loads a new plugin that isnt normally included
 with FMOD Ex, which is output_mp3.dll.  When this is loaded, it can be chosen as an output
 mode, for realtime encoding of audio output into the mp3 format.
===============================================================================================*/

using System;
using System.Text;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace useplugins
{
	public class Form1 : System.Windows.Forms.Form
	{
        private FMOD.System     system  = null;
        private FMOD.Sound      sound   = null;
        private FMOD.Channel    channel = null;

        private System.Windows.Forms.Label label;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.GroupBox codecBox;
        private System.Windows.Forms.GroupBox dspBox;
        private System.Windows.Forms.GroupBox outputBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button playButton;
        private System.Windows.Forms.ListBox codecList;
        private System.Windows.Forms.ListBox dspList;
        private System.Windows.Forms.ListBox outputList;
        private System.Windows.Forms.Timer timer;
        private System.ComponentModel.IContainer components;

		public Form1()
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
            this.components = new System.ComponentModel.Container();
            this.label = new System.Windows.Forms.Label();
            this.exit_button = new System.Windows.Forms.Button();
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.codecBox = new System.Windows.Forms.GroupBox();
            this.codecList = new System.Windows.Forms.ListBox();
            this.dspBox = new System.Windows.Forms.GroupBox();
            this.dspList = new System.Windows.Forms.ListBox();
            this.outputBox = new System.Windows.Forms.GroupBox();
            this.outputList = new System.Windows.Forms.ListBox();
            this.label1 = new System.Windows.Forms.Label();
            this.playButton = new System.Windows.Forms.Button();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.codecBox.SuspendLayout();
            this.dspBox.SuspendLayout();
            this.outputBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(60, 16);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 8;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(156, 488);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 15;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 523);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(384, 24);
            this.statusBar.TabIndex = 18;
            // 
            // codecBox
            // 
            this.codecBox.Controls.Add(this.codecList);
            this.codecBox.Location = new System.Drawing.Point(8, 56);
            this.codecBox.Name = "codecBox";
            this.codecBox.Size = new System.Drawing.Size(368, 184);
            this.codecBox.TabIndex = 19;
            this.codecBox.TabStop = false;
            this.codecBox.Text = "Codec plugins";
            // 
            // codecList
            // 
            this.codecList.Location = new System.Drawing.Point(8, 16);
            this.codecList.Name = "codecList";
            this.codecList.Size = new System.Drawing.Size(352, 160);
            this.codecList.TabIndex = 0;
            // 
            // dspBox
            // 
            this.dspBox.Controls.Add(this.dspList);
            this.dspBox.Location = new System.Drawing.Point(8, 248);
            this.dspBox.Name = "dspBox";
            this.dspBox.Size = new System.Drawing.Size(368, 80);
            this.dspBox.TabIndex = 20;
            this.dspBox.TabStop = false;
            this.dspBox.Text = "DSP plugins";
            // 
            // dspList
            // 
            this.dspList.Location = new System.Drawing.Point(8, 16);
            this.dspList.Name = "dspList";
            this.dspList.Size = new System.Drawing.Size(352, 56);
            this.dspList.TabIndex = 0;
            // 
            // outputBox
            // 
            this.outputBox.Controls.Add(this.outputList);
            this.outputBox.Location = new System.Drawing.Point(8, 336);
            this.outputBox.Name = "outputBox";
            this.outputBox.Size = new System.Drawing.Size(368, 104);
            this.outputBox.TabIndex = 21;
            this.outputBox.TabStop = false;
            this.outputBox.Text = "Output Plugins";
            // 
            // outputList
            // 
            this.outputList.Location = new System.Drawing.Point(8, 16);
            this.outputList.Name = "outputList";
            this.outputList.Size = new System.Drawing.Size(352, 82);
            this.outputList.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(16, 456);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(216, 16);
            this.label1.TabIndex = 22;
            this.label1.Text = "Select an output plugin and press \'Play\'";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // playButton
            // 
            this.playButton.Location = new System.Drawing.Point(232, 448);
            this.playButton.Name = "playButton";
            this.playButton.Size = new System.Drawing.Size(136, 32);
            this.playButton.TabIndex = 23;
            this.playButton.Text = "Play";
            this.playButton.Click += new System.EventHandler(this.playButton_Click);
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(384, 547);
            this.Controls.Add(this.playButton);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.outputBox);
            this.Controls.Add(this.dspBox);
            this.Controls.Add(this.codecBox);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.label);
            this.Name = "Form1";
            this.Text = "Use Plugins Example";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.codecBox.ResumeLayout(false);
            this.dspBox.ResumeLayout(false);
            this.outputBox.ResumeLayout(false);
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

        private void Form1_Load(object sender, System.EventArgs e)
        {
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

            /*
                 Set the source directory for all of the FMOD plugins.
             */
            result = system.setPluginPath("../../../../api/plugins");
            ERRCHECK(result);

            /*
                Load up an extra plugin that is not normally used by FMOD.
            */
            FMOD.PLUGINTYPE plugintype = FMOD.PLUGINTYPE.CODEC;
            int             index = 0;

            result = system.loadPlugin("output_mp3.dll", ref plugintype, ref index);
            if (result == FMOD.RESULT.ERR_FILE_NOTFOUND)
            {
                /*
                    If it isn't in the same directory, try for the plugin directory.
                */
                result = system.loadPlugin("../../../../examples/plugin_dev/output_mp3/output_mp3.dll", ref plugintype, ref index);
                ERRCHECK(result);
            }

            /*
                Display plugins
            */
            {
                int num = 0;
                StringBuilder name = new StringBuilder(256);

                result = system.getNumPlugins(FMOD.PLUGINTYPE.CODEC, ref num);
                ERRCHECK(result);
                for (int count = 0; count < num; count++)
                {
                    result = system.getPluginInfo(FMOD.PLUGINTYPE.CODEC, count, name, name.Capacity, ref version); 
                    ERRCHECK(result);
                    
                    codecList.Items.Add(name);
                }

                result = system.getNumPlugins(FMOD.PLUGINTYPE.DSP, ref num);
                ERRCHECK(result);
                for (int count = 0; count < num; count++)
                {
                    result = system.getPluginInfo(FMOD.PLUGINTYPE.DSP, count, name, name.Capacity, ref version);
                    ERRCHECK(result);

                    dspList.Items.Add(name);
                }

                result = system.getNumPlugins(FMOD.PLUGINTYPE.OUTPUT, ref num);
                ERRCHECK(result);
                for (int count = 0; count < num; count++)
                {
                    result = system.getPluginInfo(FMOD.PLUGINTYPE.OUTPUT, count, name, name.Capacity, ref version);
                    ERRCHECK(result);

                    outputList.Items.Add(name);
                }
            }
        }

        private void playButton_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT            result;

            int selectedplugin = 0;

            /*
                Get the output plugin that was selected
            */
            for (int count = 0; count < outputList.Items.Count; count++)
            {
                if(outputList.GetSelected(count))
                {
                    selectedplugin = count;
                    break;
                }
            }

            /*
                Initialise system and play sound
            */
            result = system.setOutputByPlugin(selectedplugin);
            ERRCHECK(result);

            result = system.init(32, FMOD.INITFLAG.NORMAL, (IntPtr)null);
            ERRCHECK(result);

            result = system.createSound("../../examples/media/wave.mp3", (FMOD.MODE.SOFTWARE | FMOD.MODE.CREATESTREAM), ref sound);
            ERRCHECK(result);

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound, false, ref channel);
            ERRCHECK(result);
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            uint        ms              = 0;
            uint        lenms           = 0;
            bool        paused          = false;
            bool        playing         = false;
            int         channelsplaying = 0;

            if (channel != null)
            {
                FMOD.Sound  currentsound    = null;

                result = channel.isPlaying(ref playing);
                if ((result != FMOD.RESULT.OK) && (result != FMOD.RESULT.ERR_INVALID_HANDLE) && (result != FMOD.RESULT.ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                result = channel.getPaused(ref paused);
                if ((result != FMOD.RESULT.OK) && (result != FMOD.RESULT.ERR_INVALID_HANDLE) && (result != FMOD.RESULT.ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                result = channel.getPosition(ref ms, FMOD.TIMEUNIT.MS);
                if ((result != FMOD.RESULT.OK) && (result != FMOD.RESULT.ERR_INVALID_HANDLE) && (result != FMOD.RESULT.ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }
       
                channel.getCurrentSound(ref currentsound);
                if (currentsound != null)
                {
                    result = currentsound.getLength(ref lenms, FMOD.TIMEUNIT.MS);
                    if ((result != FMOD.RESULT.OK) && (result != FMOD.RESULT.ERR_INVALID_HANDLE) && (result != FMOD.RESULT.ERR_CHANNEL_STOLEN))
                    {
                        ERRCHECK(result);
                    }
                }

                system.getChannelsPlaying(ref channelsplaying);
            }
            
            statusBar.Text = "Time " + (ms / 1000 / 60) + ":" + (ms / 1000 % 60) + ":" + (ms / 10 % 100) + "/" + (lenms / 1000 / 60) + ":" + (lenms / 1000 % 60) + ":" + (lenms / 10 % 100) + " " + (paused ? "Paused " : playing ? "Playing" : "Stopped") + " Channels Playing " + channelsplaying;

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
