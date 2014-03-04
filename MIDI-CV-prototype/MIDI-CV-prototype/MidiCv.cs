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
        }

        void timer1_Tick(object sender, EventArgs e)
        {
            if (!serialPort.IsOpen)
                return;
            var rec = serialPort.ReadExisting();
            tbReceived.Text += rec ;            
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
            serialPort.PortName = cbSerialSelect.Text;
            if (!serialPort.IsOpen)
            {
                serialPort.Open();
                ///serialPort.DataReceived +=serialPort_DataReceived;
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
    }
}
