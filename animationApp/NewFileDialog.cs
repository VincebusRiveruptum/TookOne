using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace animationApp {
    public partial class NewFileDialog : Form {
        private Main main;
        public NewFileDialog(Main main) {
            this.main = main;
            InitializeComponent();
        }

        private void NewFileDialog_Load(object sender, EventArgs e) {
            colorDepthCombo.Items.AddRange(new string[] {
                "1-bit monocrome",
                "4-bit 16 Colors",
                "8-bit 256 Color",
                "16-bit Color",
                "24-bit Color"
            });
        }

        private void createButton_Click(object sender, EventArgs e) {
            // We create the new file
            createFile();

        }

        private void colorDepthCombo_SelectedIndexChanged(object sender, EventArgs e) {

        }


        private void numericUpDown1_ValueChanged(object sender, EventArgs e) {
        }

        private void cancelButton_Click_1(object sender, EventArgs e) {

        }

        public void createFile() {
            // We create the new file
            if(widthNumeric.Value > 1 && heightNumeric.Value > 1) {
                main.SetEditor(new Editor((int)widthNumeric.Value, (int)heightNumeric.Value, colorDepthCombo.SelectedIndex));
                main.unlockToolStripItems();
                this.Close();
            } else {
                MessageBox.Show("Width and Height should be over 1", "Bad size", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void numericUpDown1_ValueChanged_1(object sender, EventArgs e) {

        }

        private void cancelButton_Click(object sender, EventArgs e) {

        }
    }
}
