/*===============================================================================================
 Pitch detection example.
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example combines recording with spectrum analysis to determine the pitch of the sound 
 being recorded.
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Text;
using System.Threading;
using System.Runtime.InteropServices;

namespace pitchdetection
{
	public class PitchDetection : System.Windows.Forms.Form
	{
        private string[] note =
        {
            "C 0", "C#0", "D 0", "D#0", "E 0", "F 0", "F#0", "G 0", "G#0", "A 0", "A#0", "B 0",  
            "C 1", "C#1", "D 1", "D#1", "E 1", "F 1", "F#1", "G 1", "G#1", "A 1", "A#1", "B 1",  
            "C 2", "C#2", "D 2", "D#2", "E 2", "F 2", "F#2", "G 2", "G#2", "A 2", "A#2", "B 2",  
            "C 3", "C#3", "D 3", "D#3", "E 3", "F 3", "F#3", "G 3", "G#3", "A 3", "A#3", "B 3",  
            "C 4", "C#4", "D 4", "D#4", "E 4", "F 4", "F#4", "G 4", "G#4", "A 4", "A#4", "B 4",  
            "C 5", "C#5", "D 5", "D#5", "E 5", "F 5", "F#5", "G 5", "G#5", "A 5", "A#5", "B 5",  
            "C 6", "C#6", "D 6", "D#6", "E 6", "F 6", "F#6", "G 6", "G#6", "A 6", "A#6", "B 6",  
            "C 7", "C#7", "D 7", "D#7", "E 7", "F 7", "F#7", "G 7", "G#7", "A 7", "A#7", "B 7",  
            "C 8", "C#8", "D 8", "D#8", "E 8", "F 8", "F#8", "G 8", "G#8", "A 8", "A#8", "B 8",  
            "C 9", "C#9", "D 9", "D#9", "E 9", "F 9", "F#9", "G 9", "G#9", "A 9", "A#9", "B 9"
        };

        private float[] notefreq =
        {
            16.35f,   17.32f,   18.35f,   19.45f,    20.60f,    21.83f,    23.12f,    24.50f,    25.96f,    27.50f,    29.14f,    30.87f, 
            32.70f,   34.65f,   36.71f,   38.89f,    41.20f,    43.65f,    46.25f,    49.00f,    51.91f,    55.00f,    58.27f,    61.74f, 
            65.41f,   69.30f,   73.42f,   77.78f,    82.41f,    87.31f,    92.50f,    98.00f,   103.83f,   110.00f,   116.54f,   123.47f, 
            130.81f,  138.59f,  146.83f,  155.56f,   164.81f,   174.61f,   185.00f,   196.00f,   207.65f,   220.00f,   233.08f,   246.94f, 
            261.63f,  277.18f,  293.66f,  311.13f,   329.63f,   349.23f,   369.99f,   392.00f,   415.30f,   440.00f,   466.16f,   493.88f, 
            523.25f,  554.37f,  587.33f,  622.25f,   659.26f,   698.46f,   739.99f,   783.99f,   830.61f,   880.00f,   932.33f,   987.77f, 
            1046.50f, 1108.73f, 1174.66f, 1244.51f,  1318.51f,  1396.91f,  1479.98f,  1567.98f,  1661.22f,  1760.00f,  1864.66f,  1975.53f, 
            2093.00f, 2217.46f, 2349.32f, 2489.02f,  2637.02f,  2793.83f,  2959.96f,  3135.96f,  3322.44f,  3520.00f,  3729.31f,  3951.07f, 
            4186.01f, 4434.92f, 4698.64f, 4978.03f,  5274.04f,  5587.65f,  5919.91f,  6271.92f,  6644.87f,  7040.00f,  7458.62f,  7902.13f, 
            8372.01f, 8869.84f, 9397.27f, 9956.06f, 10548.08f, 11175.30f, 11839.82f, 12543.85f, 13289.75f, 14080.00f, 14917.24f, 15804.26f
        };

        private const int   OUTPUTRATE       = 48000;
        private const int   SPECTRUMSIZE     = 8192;
        private const float SPECTRUMRANGE    = ((float)OUTPUTRATE / 2.0f);     /* 0 to nyquist */
        private const float BINSIZE          = (SPECTRUMRANGE / (float)SPECTRUMSIZE);

        private FMOD.System          system  = null;
        private FMOD.Sound           sound   = null;
        private FMOD.Channel         channel = null;
        private int                  outputfreq = 0;
        private int                  bin     = 0;

        private System.Windows.Forms.Label label;
        private System.Windows.Forms.ComboBox comboBoxOutput;
        private System.Windows.Forms.ComboBox comboBoxPlayback;
        private System.Windows.Forms.ComboBox comboBoxRecord;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.Timer timer;
        private System.ComponentModel.IContainer components;

		public PitchDetection()
		{
			InitializeComponent();
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
                FMOD.RESULT result;

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
            this.comboBoxOutput = new System.Windows.Forms.ComboBox();
            this.comboBoxPlayback = new System.Windows.Forms.ComboBox();
            this.comboBoxRecord = new System.Windows.Forms.ComboBox();
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.exit_button = new System.Windows.Forms.Button();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(64, 8);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 7;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // comboBoxOutput
            // 
            this.comboBoxOutput.Location = new System.Drawing.Point(72, 48);
            this.comboBoxOutput.Name = "comboBoxOutput";
            this.comboBoxOutput.Size = new System.Drawing.Size(248, 21);
            this.comboBoxOutput.TabIndex = 8;
            this.comboBoxOutput.Text = "Select OUTPUT type";
            this.comboBoxOutput.SelectedIndexChanged += new System.EventHandler(this.comboBoxOutput_SelectedIndexChanged);
            // 
            // comboBoxPlayback
            // 
            this.comboBoxPlayback.Location = new System.Drawing.Point(72, 72);
            this.comboBoxPlayback.Name = "comboBoxPlayback";
            this.comboBoxPlayback.Size = new System.Drawing.Size(248, 21);
            this.comboBoxPlayback.TabIndex = 9;
            this.comboBoxPlayback.Text = "Select PLAYBACK driver";
            this.comboBoxPlayback.SelectedIndexChanged += new System.EventHandler(this.comboBoxPlayback_SelectedIndexChanged);
            // 
            // comboBoxRecord
            // 
            this.comboBoxRecord.Location = new System.Drawing.Point(72, 96);
            this.comboBoxRecord.Name = "comboBoxRecord";
            this.comboBoxRecord.Size = new System.Drawing.Size(248, 21);
            this.comboBoxRecord.TabIndex = 10;
            this.comboBoxRecord.Text = "Select RECORD driver";
            this.comboBoxRecord.SelectedIndexChanged += new System.EventHandler(this.comboBoxRecord_SelectedIndexChanged);
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 155);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(392, 24);
            this.statusBar.TabIndex = 17;
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(160, 128);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 18;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // PitchDetection
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(392, 179);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.comboBoxRecord);
            this.Controls.Add(this.comboBoxPlayback);
            this.Controls.Add(this.comboBoxOutput);
            this.Controls.Add(this.label);
            this.Name = "PitchDetection";
            this.Text = "Pitch Detection Example";
            this.Load += new System.EventHandler(this.PitchDetection_Load);
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new PitchDetection());
		}

        private void PitchDetection_Load(object sender, System.EventArgs e)
        {
            int             numdrivers = 0;
            uint            version = 0;
            FMOD.RESULT     result;
            StringBuilder   drivername = new StringBuilder(256);
            FMOD.GUID       guid = new FMOD.GUID();

            comboBoxOutput.Enabled      = true;
            comboBoxPlayback.Enabled    = false;
            comboBoxRecord.Enabled      = false;

            /*
                Global Settings
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
                Get output modes
            */
            comboBoxOutput.Items.Add("DirectSound");
            comboBoxOutput.Items.Add("Windows Multimedia WaveOut");
            comboBoxOutput.Items.Add("ASIO");

            /*
                Get Playback drivers
            */
            result = system.getNumDrivers(ref numdrivers);
            ERRCHECK(result);

            for (int count = 0; count < numdrivers; count++)
            {
                result = system.getDriverInfo(count, drivername, drivername.Capacity, ref guid);
                ERRCHECK(result);

                comboBoxPlayback.Items.Add(drivername.ToString());
            }

            /*
                Get Record drivers 
            */
            result = system.getRecordNumDrivers(ref numdrivers);
            ERRCHECK(result);

            for (int count = 0; count < numdrivers; count++)
            {
                result = system.getRecordDriverInfo(count, drivername, drivername.Capacity, ref guid);
                ERRCHECK(result);

                comboBoxRecord.Items.Add(drivername.ToString());
            }
        }

        private void comboBoxOutput_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            FMOD.RESULT result = FMOD.RESULT.OK;

            switch (comboBoxOutput.SelectedIndex)
            {
                case 0:
                    result = system.setOutput(FMOD.OUTPUTTYPE.DSOUND);
                    break;
                case 1:
                    result = system.setOutput(FMOD.OUTPUTTYPE.WINMM);
                    break;
                case 2:
                    result = system.setOutput(FMOD.OUTPUTTYPE.ASIO);
                    break;
            }
            ERRCHECK(result);

            comboBoxPlayback.Enabled = true;
        }

        private void comboBoxPlayback_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            int selected = comboBoxPlayback.SelectedIndex;

            result = system.setDriver(selected);
            ERRCHECK(result);

            comboBoxRecord.Enabled = true;
        }

        private void comboBoxRecord_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            FMOD.CREATESOUNDEXINFO exinfo = new FMOD.CREATESOUNDEXINFO();
            FMOD.RESULT result;
            FMOD.DSP_RESAMPLER  resampler = FMOD.DSP_RESAMPLER.MAX;
            int selected = comboBoxRecord.SelectedIndex;
            int temp = 0;
            FMOD.SOUND_FORMAT format = FMOD.SOUND_FORMAT.NONE;
            IntPtr tempptr = new IntPtr();

            result = system.setSoftwareFormat(OUTPUTRATE, FMOD.SOUND_FORMAT.PCM16, 1, 0, 0);
            ERRCHECK(result);

            result = system.init(32, FMOD.INITFLAG.NORMAL, (IntPtr)null);
            ERRCHECK(result);

            result = system.getSoftwareFormat(ref outputfreq, ref format, ref temp, ref temp, ref resampler, ref temp);
            ERRCHECK(result);

            /*
                Create a sound to record to.
            */
            exinfo.cbsize           = Marshal.SizeOf(exinfo);
            exinfo.numchannels      = 1;
            exinfo.format           = FMOD.SOUND_FORMAT.PCM16;
            exinfo.defaultfrequency = OUTPUTRATE;
            exinfo.length           = (uint)(exinfo.defaultfrequency * 2 * exinfo.numchannels * 5);
    
            result = system.createSound((string)null, (FMOD.MODE._2D | FMOD.MODE.SOFTWARE | FMOD.MODE.LOOP_NORMAL | FMOD.MODE.OPENUSER), ref exinfo, ref sound);
            ERRCHECK(result);

            comboBoxOutput.Enabled = false;
            comboBoxPlayback.Enabled = false;
            comboBoxRecord.Enabled = false;

            /*
                Start recording 
            */
            result = system.recordStart(selected, sound, true);
            ERRCHECK(result);
    
            Thread.Sleep(200);      /* Give it some time to record something */
    
            result = system.playSound(FMOD.CHANNELINDEX.REUSE, sound, false, ref channel);
            ERRCHECK(result);

            /* Dont hear what is being recorded otherwise it will feedback.  Spectrum analysis is done before volume scaling in the DSP chain */
            result = channel.setVolume(0);
            ERRCHECK(result);
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            if (channel != null)
            {
                FMOD.RESULT result;
                float[] spectrum = new float[SPECTRUMSIZE];
                float   dominanthz = 0;
                float   max;
                int     dominantnote = 0;
                int     count = 0;

                result = channel.getSpectrum(spectrum, SPECTRUMSIZE, 0, FMOD.DSP_FFT_WINDOW.TRIANGLE);
                ERRCHECK(result);

                max = 0;

                for (count = 0; count < SPECTRUMSIZE; count++)
                {
                    if (spectrum[count] > 0.01f && spectrum[count] > max)
                    {
                        max = spectrum[count];
                        bin = count;
                    }
                }        

                dominanthz  = (float)bin * BINSIZE;       /* dominant frequency min */

                dominantnote = 0;
                for (count = 0; count < 120; count++)
                {
                    if (dominanthz >= notefreq[count] && dominanthz < notefreq[count + 1])
                    {
                        // which is it closer to.  This note or the next note
                        if (Math.Abs(dominanthz - notefreq[count]) < Math.Abs(dominanthz - notefreq[count+1]))
                        {
                            dominantnote = count;
                        }
                        else
                        {
                            dominantnote = count + 1;
                        }
                        break;
                    }
                }

                statusBar.Text = "Detected rate : " + dominanthz + " -> " + (((float)bin + 0.99f) * BINSIZE) + " Detected musical note : " + note[dominantnote] + " (" + notefreq[dominantnote]+")";
            }

            if (system != null)
            {
                system.update();
            }
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
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
