/*===============================================================================================
 Record example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how to record a sound, then write it to a wav file.
 It then shows how to play a sound while it is being recorded to.  Because it is recording, the
 sound playback has to be delayed a little bit so that the playback doesn't play part of the
 buffer that is still being written to.
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;

namespace recording
{
	public class Recording : System.Windows.Forms.Form
	{
        private FMOD.System          system  = null;
        private FMOD.Sound           sound   = null;
        private FMOD.Channel         channel = null;
        private bool                 looping = false;
		private FMOD.GUID			 guid = new FMOD.GUID();
        
        private System.Windows.Forms.Label label;
        private System.Windows.Forms.ComboBox comboBoxOutput;
        private System.Windows.Forms.ComboBox comboBoxPlayback;
        private System.Windows.Forms.ComboBox comboBoxRecord;
        private System.Windows.Forms.Button recordButton;
        private System.Windows.Forms.Button playButton;
        private System.Windows.Forms.Button loopButton;
        private System.Windows.Forms.Button stopButton;
        private System.Windows.Forms.Button saveButton;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Timer timer;
        private System.ComponentModel.IContainer components;

		public Recording()
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
            this.comboBoxOutput = new System.Windows.Forms.ComboBox();
            this.comboBoxPlayback = new System.Windows.Forms.ComboBox();
            this.comboBoxRecord = new System.Windows.Forms.ComboBox();
            this.recordButton = new System.Windows.Forms.Button();
            this.playButton = new System.Windows.Forms.Button();
            this.loopButton = new System.Windows.Forms.Button();
            this.stopButton = new System.Windows.Forms.Button();
            this.saveButton = new System.Windows.Forms.Button();
            this.exit_button = new System.Windows.Forms.Button();
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(14, 8);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 6;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // comboBoxOutput
            // 
            this.comboBoxOutput.Location = new System.Drawing.Point(24, 48);
            this.comboBoxOutput.Name = "comboBoxOutput";
            this.comboBoxOutput.Size = new System.Drawing.Size(248, 21);
            this.comboBoxOutput.TabIndex = 7;
            this.comboBoxOutput.Text = "Select OUTPUT type";
            this.comboBoxOutput.SelectedIndexChanged += new System.EventHandler(this.comboBoxOutput_SelectedIndexChanged);
            // 
            // comboBoxPlayback
            // 
            this.comboBoxPlayback.Location = new System.Drawing.Point(24, 72);
            this.comboBoxPlayback.Name = "comboBoxPlayback";
            this.comboBoxPlayback.Size = new System.Drawing.Size(248, 21);
            this.comboBoxPlayback.TabIndex = 8;
            this.comboBoxPlayback.Text = "Select PLAYBACK driver";
            this.comboBoxPlayback.SelectedIndexChanged += new System.EventHandler(this.comboBoxPlayback_SelectedIndexChanged);
            // 
            // comboBoxRecord
            // 
            this.comboBoxRecord.Location = new System.Drawing.Point(24, 96);
            this.comboBoxRecord.Name = "comboBoxRecord";
            this.comboBoxRecord.Size = new System.Drawing.Size(248, 21);
            this.comboBoxRecord.TabIndex = 9;
            this.comboBoxRecord.Text = "Select RECORD driver";
            this.comboBoxRecord.SelectedIndexChanged += new System.EventHandler(this.comboBoxRecord_SelectedIndexChanged);
            // 
            // recordButton
            // 
            this.recordButton.Location = new System.Drawing.Point(16, 128);
            this.recordButton.Name = "recordButton";
            this.recordButton.Size = new System.Drawing.Size(264, 32);
            this.recordButton.TabIndex = 10;
            this.recordButton.Text = "Record a 5 second segment of audio";
            this.recordButton.Click += new System.EventHandler(this.recordButton_Click);
            // 
            // playButton
            // 
            this.playButton.Location = new System.Drawing.Point(16, 168);
            this.playButton.Name = "playButton";
            this.playButton.Size = new System.Drawing.Size(264, 32);
            this.playButton.TabIndex = 11;
            this.playButton.Text = "Play the 5 second segment of audio";
            this.playButton.Click += new System.EventHandler(this.playButton_Click);
            // 
            // loopButton
            // 
            this.loopButton.Location = new System.Drawing.Point(16, 208);
            this.loopButton.Name = "loopButton";
            this.loopButton.Size = new System.Drawing.Size(264, 32);
            this.loopButton.TabIndex = 12;
            this.loopButton.Text = "Turn looping on/off";
            this.loopButton.Click += new System.EventHandler(this.loopButton_Click);
            // 
            // stopButton
            // 
            this.stopButton.Location = new System.Drawing.Point(16, 248);
            this.stopButton.Name = "stopButton";
            this.stopButton.Size = new System.Drawing.Size(264, 32);
            this.stopButton.TabIndex = 13;
            this.stopButton.Text = "Stop recording and playback";
            this.stopButton.Click += new System.EventHandler(this.stopButton_Click);
            // 
            // saveButton
            // 
            this.saveButton.Location = new System.Drawing.Point(16, 288);
            this.saveButton.Name = "saveButton";
            this.saveButton.Size = new System.Drawing.Size(264, 32);
            this.saveButton.TabIndex = 14;
            this.saveButton.Text = "Save the 5 second segment to a wav file";
            this.saveButton.Click += new System.EventHandler(this.saveButton_Click);
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(110, 328);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 15;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 355);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(292, 24);
            this.statusBar.TabIndex = 16;
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // Recording
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(292, 379);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.saveButton);
            this.Controls.Add(this.stopButton);
            this.Controls.Add(this.loopButton);
            this.Controls.Add(this.playButton);
            this.Controls.Add(this.recordButton);
            this.Controls.Add(this.comboBoxRecord);
            this.Controls.Add(this.comboBoxPlayback);
            this.Controls.Add(this.comboBoxOutput);
            this.Controls.Add(this.label);
            this.Name = "Recording";
            this.Text = "Recording Example";
            this.Load += new System.EventHandler(this.Recording_Load);
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new Recording());
		}

        private void Recording_Load(object sender, System.EventArgs e)
        {
            uint            version = 0;
            FMOD.RESULT     result;
            StringBuilder   drivername = new StringBuilder(256);

            comboBoxOutput.Enabled      = true;
            comboBoxPlayback.Enabled    = false;
            comboBoxRecord.Enabled      = false;

            recordButton.Enabled = false;
            playButton.Enabled   = false;
            saveButton.Enabled   = false;
            stopButton.Enabled   = false;

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
                Get output modes
            */
            comboBoxOutput.Items.Add("DirectSound");
            comboBoxOutput.Items.Add("Windows Multimedia WaveOut");
            comboBoxOutput.Items.Add("ASIO");
        }

        private void comboBoxOutput_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            FMOD.RESULT result = FMOD.RESULT.OK;
            StringBuilder   drivername = new StringBuilder(256);
            int numdrivers = 0;

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

            comboBoxPlayback.Enabled = true;
        }

        private void comboBoxPlayback_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            int selected = comboBoxPlayback.SelectedIndex;
            StringBuilder   drivername = new StringBuilder(256);
            int numdrivers = 0;

            result = system.setDriver(selected);
            ERRCHECK(result);

            comboBoxRecord.Enabled = true;

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

        private void comboBoxRecord_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            FMOD.CREATESOUNDEXINFO exinfo = new FMOD.CREATESOUNDEXINFO();
            FMOD.RESULT result;
            int selected = comboBoxRecord.SelectedIndex;

            result = system.setRecordDriver(selected);
            ERRCHECK(result);

            comboBoxOutput.Enabled = false;
            comboBoxPlayback.Enabled = false;
            comboBoxRecord.Enabled = false;

            /*
                Initialise 
            */
            result = system.init(32, FMOD.INITFLAG.NORMAL, (IntPtr)null);
            ERRCHECK(result);

            exinfo.cbsize           = Marshal.SizeOf(exinfo);
            exinfo.numchannels      = 1;
            exinfo.format           = FMOD.SOUND_FORMAT.PCM16;
            exinfo.defaultfrequency = 44100;
            exinfo.length           = (uint)(exinfo.defaultfrequency * 2 * exinfo.numchannels * 5);
 
            result = system.createSound((string)null, (FMOD.MODE._2D | FMOD.MODE.SOFTWARE | FMOD.MODE.OPENUSER), ref exinfo, ref sound);
            ERRCHECK(result);

            recordButton.Enabled = true;
            playButton.Enabled   = true;
            saveButton.Enabled   = true;
            stopButton.Enabled   = true;
        }

        private void recordButton_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = system.recordStart(sound, looping);
            ERRCHECK(result);
        }

        private void playButton_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            if (looping)
            {
                result = sound.setMode(FMOD.MODE.LOOP_NORMAL);
            }
            else
            {
                result = sound.setMode(FMOD.MODE.LOOP_OFF);
            }
            ERRCHECK(result);

            result = system.playSound(FMOD.CHANNELINDEX.REUSE, sound, false, ref channel);
            ERRCHECK(result);
        }

        private void loopButton_Click(object sender, System.EventArgs e)
        {
            looping = !looping;
        }

        private void stopButton_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = system.recordStop();
            ERRCHECK(result);

            if (channel != null)
            {
                channel.stop();
                channel = null;
            }
        }

        private void saveButton_Click(object sender, System.EventArgs e)
        {
            SaveToWav();
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            if (system != null && sound != null)
            {
                uint recordpos  = 0;
                uint playpos    = 0;
                bool recording  = false;
                bool playing    = false;

                system.isRecording(ref recording);
                system.getRecordPosition(ref recordpos);

                if (channel != null)
                {
                    channel.isPlaying(ref playing);;

                    channel.getPosition(ref playpos, FMOD.TIMEUNIT.PCM);
                }

                statusBar.Text = "State:" + (recording ? playing ? " Recording / playing " : " Recording " : playing ? " Playing " : " Idle ")
                                + " Record pos = " + recordpos + " Play pos = " + playpos + " Loop " + (looping ? "On" : "Off");
            }
            if (system != null)
            {
                system.update();
            }
        }
    
    
        /*
            WAV Structures 
        */
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        struct RiffChunk
        {
            [MarshalAs(UnmanagedType.ByValArray,SizeConst=4)]
            public char[] id;
            public int 	  size;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        struct FmtChunk
        {
            public RiffChunk    chunk;
            public ushort	    wFormatTag;    /* format type  */
            public ushort	    nChannels;    /* number of channels (i.e. mono, stereo...)  */
            public uint	        nSamplesPerSec;    /* sample rate  */
            public uint	        nAvgBytesPerSec;    /* for buffer estimation  */
            public ushort	    nBlockAlign;    /* block size of data  */
            public ushort	    wBitsPerSample;    /* number of bits per sample of mono data */
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        struct DataChunk
        {
            public RiffChunk   chunk;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        struct WavHeader
        {
            public RiffChunk   chunk;
            [MarshalAs(UnmanagedType.ByValArray,SizeConst=4)]
            public char[]      rifftype;
        }
        
        private void SaveToWav()
        {
            FileStream          fs  = null;
            int                 channels = 0, bits = 0;
            float               rate = 0;
            IntPtr              ptr1 = new IntPtr(), ptr2 = new IntPtr();
            uint                lenbytes = 0, len1 = 0, len2 = 0;
            FMOD.SOUND_TYPE     type = FMOD.SOUND_TYPE.WAV;
            FMOD.SOUND_FORMAT   format = FMOD.SOUND_FORMAT.NONE;
            int                 temp1 = 0;
            float               temp3 = 0;

            if (sound == null)
            {
                return;
            }

            sound.getFormat  (ref type, ref format, ref channels, ref bits);
            sound.getDefaults(ref rate, ref temp3, ref temp3, ref temp1);
            sound.getLength  (ref lenbytes, FMOD.TIMEUNIT.PCMBYTES);

            FmtChunk            fmtChunk  = new FmtChunk();
            DataChunk           dataChunk = new DataChunk();
            WavHeader           wavHeader = new WavHeader();
            RiffChunk           riffChunk = new RiffChunk();

            fmtChunk.chunk = new RiffChunk();
            fmtChunk.chunk.id = new char[4];
            fmtChunk.chunk.id[0]     = 'f';
            fmtChunk.chunk.id[1]     = 'm';
            fmtChunk.chunk.id[2]     = 't';
            fmtChunk.chunk.id[3]     = ' ';
            fmtChunk.chunk.size      = Marshal.SizeOf(fmtChunk) - Marshal.SizeOf(riffChunk);
            fmtChunk.wFormatTag      = 1;
            fmtChunk.nChannels       = (ushort)channels;
            fmtChunk.nSamplesPerSec  = (uint)rate;
            fmtChunk.nAvgBytesPerSec = (uint)(rate * channels * bits / 8);
            fmtChunk.nBlockAlign     = (ushort)(1 * channels * bits / 8);
            fmtChunk.wBitsPerSample  = (ushort)bits;

            dataChunk.chunk = new RiffChunk();
            dataChunk.chunk.id = new char[4];
            dataChunk.chunk.id[0]    = 'd';
            dataChunk.chunk.id[1]    = 'a';
            dataChunk.chunk.id[2]    = 't';
            dataChunk.chunk.id[3]    = 'a';
            dataChunk.chunk.size     = (int)lenbytes;

            wavHeader.chunk = new RiffChunk();
            wavHeader.chunk.id = new char[4];
            wavHeader.chunk.id[0]   = 'R';
            wavHeader.chunk.id[1]   = 'I';
            wavHeader.chunk.id[2]   = 'F';
            wavHeader.chunk.id[3]   = 'F';
            wavHeader.chunk.size    = (int)(Marshal.SizeOf(fmtChunk) + Marshal.SizeOf(riffChunk) + lenbytes);
            wavHeader.rifftype = new char[4];
            wavHeader.rifftype[0]   = 'W';
            wavHeader.rifftype[1]   = 'A';
            wavHeader.rifftype[2]   = 'V';
            wavHeader.rifftype[3]   = 'E';

            fs = new FileStream("record.wav", FileMode.Create, FileAccess.Write);

            /*
                Write out the WAV header.
            */
            IntPtr wavHeaderPtr = Marshal.AllocHGlobal(Marshal.SizeOf(wavHeader));
            IntPtr fmtChunkPtr  = Marshal.AllocHGlobal(Marshal.SizeOf(fmtChunk));
            IntPtr dataChunkPtr = Marshal.AllocHGlobal(Marshal.SizeOf(dataChunk));
            byte   []wavHeaderBytes = new byte[Marshal.SizeOf(wavHeader)];
            byte   []fmtChunkBytes  = new byte[Marshal.SizeOf(fmtChunk)];
            byte   []dataChunkBytes = new byte[Marshal.SizeOf(dataChunk)];

            Marshal.StructureToPtr(wavHeader, wavHeaderPtr, false);
            Marshal.Copy(wavHeaderPtr, wavHeaderBytes, 0, Marshal.SizeOf(wavHeader));

            Marshal.StructureToPtr(fmtChunk, fmtChunkPtr, false);
            Marshal.Copy(fmtChunkPtr, fmtChunkBytes, 0, Marshal.SizeOf(fmtChunk));

            Marshal.StructureToPtr(dataChunk, dataChunkPtr, false);
            Marshal.Copy(dataChunkPtr, dataChunkBytes, 0, Marshal.SizeOf(dataChunk));

            fs.Write(wavHeaderBytes, 0, Marshal.SizeOf(wavHeader));
            fs.Write(fmtChunkBytes, 0, Marshal.SizeOf(fmtChunk));
            fs.Write(dataChunkBytes, 0, Marshal.SizeOf(dataChunk));

            /*
                Lock the sound to get access to the raw data.
            */
            sound.@lock(0, lenbytes, ref ptr1, ref ptr2, ref len1, ref len2);

            /*
                Write it to disk.
            */
            byte []rawdata = new byte[len1];
            
            Marshal.Copy(ptr1, rawdata, 0, (int)len1);

            fs.Write(rawdata, 0, (int)len1);

            /*
                Unlock the sound to allow FMOD to use it again.
            */
            sound.unlock(ptr1, ptr2, len1, len2);

            fs.Close();

            MessageBox.Show("Written to record.wav");
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
