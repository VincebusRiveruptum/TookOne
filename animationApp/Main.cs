using System.IO;
using System.Security;
using System.Windows.Forms;

namespace animationApp {
    public partial class Main : Form {
        private Editor editor;
        public Main() {
            InitializeComponent();
        }

        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e) {

        }

        private void quitToolStripMenuItem_Click(object sender, EventArgs e) {

        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e) {
            MessageBox.Show("This is a test app by vincebus");
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e) {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "BMP File | *.bmp";
            
            // We open a file, first we open the open file common dialog
            if(openFileDialog.ShowDialog() == DialogResult.OK) {
                try {

                    editor = new Editor(new Bitmap(openFileDialog.FileName));
                    mainPictureBox.Image = editor.getBitmap();

                } catch(SecurityException ex) {
                    MessageBox.Show(ex.Message);
                }
            }

        }

        private void textBox1_TextChanged(object sender, EventArgs e) {

        }

        private void Main_Load(object sender, EventArgs e) {
            this.Text = ("Vincebus's app test");
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e) {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "BMP File | *.bmp";

            // We open a file, first we open the open file common dialog
            if(saveFileDialog.ShowDialog() == DialogResult.OK) {
                try {
                    editor.getBitmap().Save(saveFileDialog.FileName);
      

                } catch(SecurityException ex) {
                    MessageBox.Show(ex.Message);
                }
            }

        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e) {
           
            
            // Verify if a file was already created

            // Open new file dialog.

            NewFileDialog newFileDialog = new NewFileDialog(this);
            newFileDialog.ShowDialog();

        }

        // Setters & getters
        public void SetEditor(Editor editor) {
            this.editor = editor;

            // We update the pictureBox with the new picture created
            mainPictureBox.Image = editor.getBitmap();
        }

        Editor GetEditor() {
            return this.editor;
        }
        
    }
}
