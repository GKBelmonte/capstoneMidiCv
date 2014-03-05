using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MIDI_CV_prototype
{
    public partial class MidiCv : Form
    {
        Timer timer1 = new Timer();
        public MidiCv()
        {
            InitializeComponent();
            string[] ports = System.IO.Ports.SerialPort.GetPortNames();
            cbSerialSelect.Items.Clear();
            foreach (var port in ports)
            {
                cbSerialSelect.Items.Add(port);
            }
            if (ports.Length > 0)
                cbSerialSelect.Text = ports[0];
            this.FormClosing += MidiCv_FormClosing;
            timer1.Interval = 100;
            timer1.Tick += timer1_Tick;
            timer1.Start();
            this.KeyDown += MidiCv_KeyDown;
            this.KeyUp += MidiCv_KeyUp;
            this.KeyPreview = true;
        }
        bool midi = true;
        void MidiCv_KeyUp(object sender, KeyEventArgs e)
        {

            if (e.KeyCode == Keys.A || e.KeyCode == Keys.S || e.KeyCode == Keys.D || e.KeyCode == Keys.W ||
                e.KeyCode == Keys.E || e.KeyCode == Keys.F || e.KeyCode == Keys.T || e.KeyCode == Keys.F ||
                e.KeyCode == Keys.G || e.KeyCode == Keys.Y || e.KeyCode == Keys.H || e.KeyCode == Keys.U ||
                e.KeyCode == Keys.J)
            {
                keyIsDown = false;
                if (midi)
                {
                    serialPort.Write(new byte[] { 0x80, (byte)10, 0x0F }, 0, 3);
                }
                else
                    serialPort.Write(new byte[] { 0x00 }, 0, 1);
            }
            
        }
        bool keyIsDown = false;
        void MidiCv_KeyDown(object sender, KeyEventArgs e)
        {
            if (keyIsDown)
                return;
            
            int noteNumber = 0;
            switch (e.KeyCode)
            { 
                case Keys.A:
                    noteNumber = 60;
                    break;
                case Keys.S:
                    noteNumber = 62;
                    break;
                case Keys.D:
                    noteNumber = 64;
                    break;
 
                case Keys.F:
                    noteNumber = 65;
                    break;
                case Keys.G:
                    noteNumber = 67;
                    break;
                case Keys.H:
                    noteNumber = 69;
                    break;
                case Keys.J:
                    noteNumber = 71;
                    break;
                case Keys.W:
                    noteNumber = 61;
                    break;
                case Keys.E:
                    noteNumber = 63;
                    break;
                case Keys.T:
                    noteNumber = 66;
                    break;

                case Keys.Y:
                    noteNumber = 68;
                    break;
                case Keys.U:
                    noteNumber = 70;
                    break;
                default:
                    return;
            }
            keyIsDown = true;
            try
            {
                if (midi)
                {
                    serialPort.Write(new byte[] { 0x90, (byte)noteNumber, 0x7F }, 0, 3);
                }
                else
                    serialPort.Write(new byte[] { (byte)(100.0f * (155.0 * ((float)noteNumber - 60) / 12.0f) + 100.0f) }, 0, 1);
            }
            catch (Exception err)
            {
                MessageBox.Show(string.Format("Couldn't send because {0}", err.Message), "Err", MessageBoxButtons.OK);
            }
            
            //MessageBox.Show(string.Format("Value is  {0}:{1}", noteNumber, (byte)(100.0f * (155.0 * ((float)noteNumber - 60) / 12.0f))), "debug");
        }

        void timer1_Tick(object sender, EventArgs e)
        {
            if (!serialPort.IsOpen)
                return;
            var rec = serialPort.ReadExisting();
            tbReceived.Text += rec ;
            tbReceived.SelectionStart = tbReceived.TextLength;
            if(rec != string.Empty)
                tbReceived.ScrollToCaret();
        }

        void MidiCv_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (serialPort.IsOpen)
            {
                try { serialPort.Close(); }
                catch (Exception) { }
            }
        }

        private void sniff_click(object sender, EventArgs e)
        {
            string[] ports = System.IO.Ports.SerialPort.GetPortNames();
            cbSerialSelect.Items.Clear();
            foreach (var port in ports)
            {
                cbSerialSelect.Items.Add(port);
            }
        }

        private void bSerialBegin_Click(object sender, EventArgs e)
        {
            
            if (!serialPort.IsOpen)
            {
                serialPort.PortName = cbSerialSelect.Text;
                serialPort.Open();
                bSerialBegin.Text = "Close";
                tbReceived.Text = string.Empty;
                //serialPort.DataReceived +=serialPort_DataReceived;
            }
            else if (bSerialBegin.Text == "Close")
            {
                serialPort.Close();
                bSerialBegin.Text = "Begin";
            }
            else
            {
                MessageBox.Show("Serial port is already in use", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

        }

        private void serialPort_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke(new SerialDataReceivedEventHandler (serialPort_DataReceived), new object[] { sender, e });
                return;
            }
            var rec =  serialPort.ReadLine();
            tbReceived.Text += rec + Environment.NewLine;
        }

        private void send_click(object sender, EventArgs e)
        {
            try
            {
                var x = byte.Parse(tbSendByte.Text);
                try
                {
                    serialPort.Write(new byte[] { x }, 0, 1);
                }
                catch (Exception err)
                {
                    MessageBox.Show(string.Format("Couldn't send byte: {0}",err.Message), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            catch (Exception)
            {
                MessageBox.Show("Couldn't parse byte", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (!this.KeyPreview)
            {
                this.KeyPreview = true;
            }
            else 
            {
                this.KeyPreview = false;
            }
        }
    }
}
