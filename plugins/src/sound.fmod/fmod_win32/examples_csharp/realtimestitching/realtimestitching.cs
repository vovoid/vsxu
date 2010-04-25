/*===============================================================================================
 Real-time stitching example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how you can create your own multi-subsound stream, then in realtime replace
 each the subsound as it plays them.  Using a looping sentence, it will seamlessly stich between
 2 subsounds in this example, and each time it switches to a new sound, it will replace the old
 one with another sound in our list.

 These sounds can go on forever as long as they are the same bitdepth (when decoded) and number
 of channels (ie mono / stereo).  The reason for this is the hardware channel cannot change 
 formats mid sentence, and using different hardware channels would mean it wouldn't be gapless.

===============================================================================================*/
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Runtime.InteropServices;

namespace realtimestitching
{
	public class Form1 : System.Windows.Forms.Form
	{
        private const int NUMSOUNDS = 32;

        private FMOD.System     system   = null;
        private FMOD.Sound      sound    = null;
        private FMOD.Sound[]    subsound = new FMOD.Sound[2];
        private FMOD.Channel    channel  = null;
        private int             subsoundid = 0, sentenceid = 0;
        private string[]        soundname =
        {
            "../../../../examples/media/e.ogg",   /* Ma-    */
            "../../../../examples/media/d.ogg",   /* ry     */
            "../../../../examples/media/c.ogg",   /* had    */
            "../../../../examples/media/d.ogg",   /* a      */
            "../../../../examples/media/e.ogg",   /* lit-   */
            "../../../../examples/media/e.ogg",   /* tle    */
            "../../../../examples/media/e.ogg",   /* lamb,  */
            "../../../../examples/media/e.ogg",   /* .....  */
            "../../../../examples/media/d.ogg",   /* lit-   */
            "../../../../examples/media/d.ogg",   /* tle    */
            "../../../../examples/media/d.ogg",   /* lamb,  */
            "../../../../examples/media/d.ogg",   /* .....  */
            "../../../../examples/media/e.ogg",   /* lit-   */
            "../../../../examples/media/e.ogg",   /* tle    */
            "../../../../examples/media/e.ogg",   /* lamb,  */
            "../../../../examples/media/e.ogg",   /* .....  */

            "../../../../examples/media/e.ogg",   /* Ma-    */
            "../../../../examples/media/d.ogg",   /* ry     */
            "../../../../examples/media/c.ogg",   /* had    */
            "../../../../examples/media/d.ogg",   /* a      */
            "../../../../examples/media/e.ogg",   /* lit-   */
            "../../../../examples/media/e.ogg",   /* tle    */
            "../../../../examples/media/e.ogg",   /* lamb,  */
            "../../../../examples/media/e.ogg",   /* its    */
            "../../../../examples/media/d.ogg",   /* fleece */
            "../../../../examples/media/d.ogg",   /* was    */
            "../../../../examples/media/e.ogg",   /* white  */
            "../../../../examples/media/d.ogg",   /* as     */
            "../../../../examples/media/c.ogg",   /* snow.  */
            "../../../../examples/media/c.ogg",   /* .....  */
            "../../../../examples/media/c.ogg",   /* .....  */
            "../../../../examples/media/c.ogg"    /* .....  */
        };

        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Button pauseButton;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.Label label;
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
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.pauseButton = new System.Windows.Forms.Button();
            this.exit_button = new System.Windows.Forms.Button();
            this.label = new System.Windows.Forms.Label();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 91);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(292, 24);
            this.statusBar.TabIndex = 17;

            // 
            // pauseButton
            // 
            this.pauseButton.Location = new System.Drawing.Point(8, 48);
            this.pauseButton.Name = "pauseButton";
            this.pauseButton.Size = new System.Drawing.Size(160, 32);
            this.pauseButton.TabIndex = 21;
            this.pauseButton.Text = "Pause/Resume";
            this.pauseButton.Click += new System.EventHandler(this.pauseButton_Click);
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(176, 48);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(104, 32);
            this.exit_button.TabIndex = 22;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(14, 8);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 23;
            this.label.Text = "Copyright (c) Firelight Technologies 2004/2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
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
            this.ClientSize = new System.Drawing.Size(292, 115);
            this.Controls.Add(this.label);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.pauseButton);
            this.Controls.Add(this.statusBar);
            this.Name = "Form1";
            this.Text = "Real-time Stitching Example";
            this.Load += new System.EventHandler(this.Form1_Load);
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
            FMOD.CREATESOUNDEXINFO exinfo = new FMOD.CREATESOUNDEXINFO();
            FMOD.RESULT            result;
            uint                   version = 0;

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
                Set up the FMOD_CREATESOUNDEXINFO structure for the user stream with room for 2 subsounds. (our subsound double buffer)
            */
            
            exinfo.cbsize = Marshal.SizeOf(exinfo);
            exinfo.defaultfrequency = 44100;
            exinfo.numsubsounds = 2;
            exinfo.numchannels = 1;
            exinfo.format = FMOD.SOUND_FORMAT.PCM16;

            /*
                Create the 'parent' stream that contains the substreams.  Set it to loop so that it loops between subsound 0 and 1.
            */
            result = system.createStream("", FMOD.MODE.LOOP_NORMAL | FMOD.MODE.OPENUSER, ref exinfo, ref sound);
            ERRCHECK(result);

            /*
                Add 2 of our streams as children of the parent.  They should be the same format (ie mono/stereo and bitdepth) as the parent sound.
                When subsound 0 has finished and it is playing subsound 1, we will swap subsound 0 with a new sound, and the same for when subsound 1 has finished,
                causing a continual double buffered flip, which means continuous sound.
            */
            result = system.createStream(soundname[0], FMOD.MODE.DEFAULT, ref subsound[0]);
            ERRCHECK(result);

            result = system.createStream(soundname[1], FMOD.MODE.DEFAULT, ref subsound[1]);
            ERRCHECK(result);

            result = sound.setSubSound(0, subsound[0]);
            ERRCHECK(result);

            result = sound.setSubSound(1, subsound[1]);
            ERRCHECK(result);

            /*
                Set up the gapless sentence to contain these first 2 streams.
            */
            {
                int[] soundlist = { 0, 1 };
             
                result = sound.setSubSoundSentence(soundlist, 2);
                ERRCHECK(result);
            }

            subsoundid = 0;     
            sentenceid = 2;     /* The next sound to be appeneded to the stream. */

            /*
                Play the sound.
            */

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound, false, ref channel);
            ERRCHECK(result);
        }

        private void pauseButton_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            bool paused = false;

            if (channel != null)
            {
                result = channel.getPaused(ref paused);
                ERRCHECK(result);
                result = channel.setPaused(!paused);
                ERRCHECK(result);
            }
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            /*
                Replace the subsound that just finished with a new subsound, to create endless seamless stitching!

                Note that this polls the currently playing subsound using the FMOD_TIMEUNIT_BUFFERED flag.  
                Remember streams are decoded / buffered ahead in advance! 
                Don't use the 'audible time' which is FMOD_TIMEUNIT_SENTENCE_SUBSOUND by itself.  When streaming, sound is 
                processed ahead of time, and things like stream buffer / sentence manipulation (as done below) is required 
                to be in 'buffered time', or else there will be synchronization problems and you might end up releasing a
                sub-sound that is still playing!
            */
            if (channel != null)
            {
                uint currentsubsoundid = 0;

                result = channel.getPosition(ref currentsubsoundid, FMOD.TIMEUNIT.SENTENCE_SUBSOUND | FMOD.TIMEUNIT.BUFFERED);
                ERRCHECK(result);

                if (currentsubsoundid != subsoundid)
                {
                    /* 
                        Release the sound that isn't playing any more. 
                    */
                    result = subsound[subsoundid].release();       
                    ERRCHECK(result);
       
                    /* 
                        Replace it with a new sound in our list.
                    */
                    result = system.createStream(soundname[sentenceid], FMOD.MODE.DEFAULT, ref subsound[subsoundid]);
                    ERRCHECK(result);

                    result = sound.setSubSound(subsoundid, subsound[subsoundid]);
                    ERRCHECK(result);

                    statusBar.Text = "Replacing subsound " + subsoundid+ " / 2 with sound " + sentenceid + " / " + NUMSOUNDS;

                    sentenceid++;
                    if (sentenceid >= NUMSOUNDS)
                    {
                        sentenceid = 0;
                    }

                    subsoundid = (int)currentsubsoundid;
                }
            }

            if (system != null)
            {
                result = system.update();
                ERRCHECK(result);
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
