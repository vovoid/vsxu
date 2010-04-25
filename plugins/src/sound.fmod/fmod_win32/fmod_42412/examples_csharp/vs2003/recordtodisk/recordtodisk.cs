/*===============================================================================================
 Record to disk example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how to do a streaming record to disk.
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

namespace recordtodisk
{
	public class recordtodisk : System.Windows.Forms.Form
	{
        private FMOD.System          system    = null;
        private FMOD.Sound           sound     = null;
        private bool                 recording = false;
        private FileStream           fs        = null;
        private uint                 soundlength   = 0;
        private uint                 lastrecordpos = 0;
        private int                  datalength    = 0;
        private int selected = 0;

        private System.Windows.Forms.Label label;
        private System.Windows.Forms.ComboBox comboBoxRecord;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Button start;
        private System.Windows.Forms.ComboBox comboBoxOutput;
        private System.Windows.Forms.Button stop;
        private System.ComponentModel.IContainer components;

		public recordtodisk()
		{
			InitializeComponent();
		}

		protected override void Dispose( bool disposing )
		{
            /*
                Write back the wav header now that we know its length.
            */
            if (recording)
            {
                recording = false;

                WriteWavHeader(datalength);
            }

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
            this.comboBoxRecord = new System.Windows.Forms.ComboBox();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.exit_button = new System.Windows.Forms.Button();
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.start = new System.Windows.Forms.Button();
            this.comboBoxOutput = new System.Windows.Forms.ComboBox();
            this.stop = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(8, 8);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 7;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // comboBoxRecord
            // 
            this.comboBoxRecord.Location = new System.Drawing.Point(16, 72);
            this.comboBoxRecord.Name = "comboBoxRecord";
            this.comboBoxRecord.Size = new System.Drawing.Size(248, 21);
            this.comboBoxRecord.TabIndex = 10;
            this.comboBoxRecord.Text = "Select RECORD driver";
            this.comboBoxRecord.SelectedIndexChanged += new System.EventHandler(this.comboBoxRecord_SelectedIndexChanged);
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(96, 144);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 16;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 171);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(272, 24);
            this.statusBar.TabIndex = 17;
            // 
            // start
            // 
            this.start.Location = new System.Drawing.Point(16, 104);
            this.start.Name = "start";
            this.start.Size = new System.Drawing.Size(128, 32);
            this.start.TabIndex = 18;
            this.start.Text = "Start recording to record.wav";
            this.start.Click += new System.EventHandler(this.start_Click);
            // 
            // comboBoxOutput
            // 
            this.comboBoxOutput.Location = new System.Drawing.Point(16, 40);
            this.comboBoxOutput.Name = "comboBoxOutput";
            this.comboBoxOutput.Size = new System.Drawing.Size(248, 21);
            this.comboBoxOutput.TabIndex = 19;
            this.comboBoxOutput.Text = "Select OUTPUT type";
            this.comboBoxOutput.SelectedIndexChanged += new System.EventHandler(this.comboBoxOutput_SelectedIndexChanged);
            // 
            // stop
            // 
            this.stop.Location = new System.Drawing.Point(152, 104);
            this.stop.Name = "stop";
            this.stop.Size = new System.Drawing.Size(104, 32);
            this.stop.TabIndex = 20;
            this.stop.Text = "Stop";
            this.stop.Click += new System.EventHandler(this.stop_Click);
            // 
            // recordtodisk
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(272, 195);
            this.Controls.Add(this.stop);
            this.Controls.Add(this.comboBoxOutput);
            this.Controls.Add(this.start);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.comboBoxRecord);
            this.Controls.Add(this.label);
            this.Name = "recordtodisk";
            this.Text = "Record To Disk Example";
            this.Load += new System.EventHandler(this.recordtodisk_Load);
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new recordtodisk());
		}

        private void recordtodisk_Load(object sender, System.EventArgs e)
        {
            uint            version = 0;
            FMOD.RESULT     result;

            comboBoxOutput.Enabled      = true;
            comboBoxRecord.Enabled      = false;
            start.Enabled               = false;

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
            FMOD.RESULT     result     = FMOD.RESULT.OK;
            StringBuilder   drivername = new StringBuilder(256);
            int             numdrivers = 0;
            FMOD.GUID       guid = new FMOD.GUID();

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
                default:
                    return;
            }
            ERRCHECK(result);

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

            comboBoxOutput.Enabled = false;
            comboBoxRecord.Enabled = true;
        }

        private void comboBoxRecord_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            FMOD.CREATESOUNDEXINFO exinfo = new FMOD.CREATESOUNDEXINFO();
            FMOD.RESULT result;
            selected = comboBoxRecord.SelectedIndex;

            comboBoxOutput.Enabled = false;
            comboBoxRecord.Enabled = false;

            /*
                Initialise 
            */
            result = system.init(32, FMOD.INITFLAG.NORMAL, (IntPtr)null);
            ERRCHECK(result);

            exinfo.cbsize           = Marshal.SizeOf(exinfo);
            exinfo.numchannels      = 2;
            exinfo.format           = FMOD.SOUND_FORMAT.PCM16;
            exinfo.defaultfrequency = 44100;
            exinfo.length           = (uint)(exinfo.defaultfrequency * 2 * exinfo.numchannels * 2);
 
            result = system.createSound((string)null, (FMOD.MODE._2D | FMOD.MODE.SOFTWARE | FMOD.MODE.OPENUSER), ref exinfo, ref sound);
            ERRCHECK(result);

            start.Enabled = true;
        }

        private void start_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = system.recordStart(selected, sound, true);
            ERRCHECK(result);

            soundlength   = 0;
            lastrecordpos = 0;
            datalength    = 0;

            fs = new FileStream("record.wav", FileMode.Create, FileAccess.Write);

            /*
                Write out the wav header.  As we don't know the length yet it will be 0.
            */
            WriteWavHeader(0);

            recording = true;

            result = sound.getLength(ref soundlength, FMOD.TIMEUNIT.PCM);
            ERRCHECK(result);

            start.Enabled = false;
        }

        private void stop_Click(object sender, System.EventArgs e)
        {
            /*
                Write back the wav header now that we know its length.
            */
            if (recording)
            {
                recording = false;

                WriteWavHeader(datalength);

                fs.Close();

                start.Enabled = true;
            }
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            if (recording)
            {
                uint recordpos = 0;

                result = system.getRecordPosition(selected, ref recordpos);
                ERRCHECK(result);

                if (recordpos != lastrecordpos)        
                {
                    IntPtr ptr1 = IntPtr.Zero, ptr2 = IntPtr.Zero;
                    int blocklength;
                    uint len1 = 0, len2 = 0;
                            
                    blocklength = (int)recordpos - (int)lastrecordpos;
                    if (blocklength < 0)
                    {
                        blocklength += (int)soundlength;
                    }

                    /*
                        Lock the sound to get access to the raw data.
                    */
                    sound.@lock(lastrecordpos * 4, (uint)blocklength * 4, ref ptr1, ref ptr2, ref len1, ref len2); /* *4 = stereo 16bit.  1 sample = 4 bytes. */

                    /*
                        Write it to disk.
                    */
                    if (ptr1 != IntPtr.Zero && len1 > 0)
                    {
                        byte[] buf = new byte[len1];

                        Marshal.Copy(ptr1, buf, 0, (int)len1);

                        datalength += (int)len1;
                        
                        fs.Write(buf, 0, (int)len1);
                        
                    }
                    if (ptr2 != IntPtr.Zero && len2 > 0)
                    {
                        byte[] buf = new byte[len2];

                        Marshal.Copy(ptr2, buf, 0, (int)len2);

                        datalength += (int)len2;
                        
                        fs.Write(buf, 0, (int)len2);
                    }

                    /*
                        Unlock the sound to allow FMOD to use it again.
                    */
                    sound.unlock(ptr1, ptr1, len1, len2);
                }

                lastrecordpos = recordpos;

                statusBar.Text = "Record buffer pos = " + recordpos + " Record time = " + datalength / 44100 / 4 / 60 + ":" + (datalength / 44100 / 4) % 60;
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
        
        private void WriteWavHeader(int length)
        {
            FMOD.SOUND_TYPE     type   = FMOD.SOUND_TYPE.UNKNOWN;
            FMOD.SOUND_FORMAT   format = FMOD.SOUND_FORMAT.NONE;
            int                 channels = 0, bits = 0, temp1 = 0;
            float               rate = 0.0f;
            float               temp = 0.0f;

            if (sound == null)
            {
                return;
            }

            sound.getFormat(ref type, ref format, ref channels, ref bits);
            sound.getDefaults(ref rate, ref temp, ref temp, ref temp1);

            fs.Seek(0, SeekOrigin.Begin);

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
            dataChunk.chunk.size     = (int)length;

            wavHeader.chunk = new RiffChunk();
            wavHeader.chunk.id = new char[4];
            wavHeader.chunk.id[0]   = 'R';
            wavHeader.chunk.id[1]   = 'I';
            wavHeader.chunk.id[2]   = 'F';
            wavHeader.chunk.id[3]   = 'F';
            wavHeader.chunk.size    = (int)(Marshal.SizeOf(fmtChunk) + Marshal.SizeOf(riffChunk) + length);
            wavHeader.rifftype = new char[4];
            wavHeader.rifftype[0]   = 'W';
            wavHeader.rifftype[1]   = 'A';
            wavHeader.rifftype[2]   = 'V';
            wavHeader.rifftype[3]   = 'E';

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
