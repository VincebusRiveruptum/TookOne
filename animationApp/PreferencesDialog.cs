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
    public partial class PreferencesDialog : Form {
        private Main main;
        public PreferencesDialog(Main main) {
            InitializeComponent();
            this.main = main;
        }

        private void PreferencesDialog_Load(object sender, EventArgs e) {
            if(this.main.getCenteredWorkspace() == true) {
                checkBox1.Checked = true;
            } else {
                checkBox1.Checked = false;
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e) {
            if(checkBox1.Checked) {
                main.workspaceCenter();
            } else {
                main.workspaceCorner();
            }
        }

        private void buttonOk_Click(object sender, EventArgs e) {
            main.updateWorkspace();
            this.Close();
        }
    }
}
