using System.IO;
using System.Security;
using System.Windows.Forms;

namespace animationApp {
    public partial class Main : Form {
        private Editor editor;
        private bool isDrawing;
        private Point lastPoint;
        private Graphics editorLayer;
        private int selectedTool;

        public Main() {
            InitializeComponent();
            this.isDrawing = false;
            this.lastPoint = new Point();
            this.selectedTool = 0;

            mainPictureBox.Enabled = false;
        }

        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e) {

        }

        private void quitToolStripMenuItem_Click(object sender, EventArgs e) {

        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e) {
            openAboutDialog();
        }

        private void newFile() {
            // Verify if a file was already created
            if(verifyOpenedFile()) {
                NewFileDialog newFileDialog = new NewFileDialog(this);
                newFileDialog.ShowDialog();
                mainPictureBox.Enabled = true;
            }
            // Open new file dialog.
        }

        public void openFile() {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "BMP File | *.bmp";

            // We open a file, first we open the open file common dialog
            if(openFileDialog.ShowDialog() == DialogResult.OK) {
                try {

                    editor = new Editor(new Bitmap(openFileDialog.FileName));
                    mainPictureBox.Image = editor.getBitmap();
                    unlockToolStripItems();
                    mainPictureBox.Enabled = true;

                } catch(SecurityException ex) {
                    MessageBox.Show(ex.Message);
                }
            }

        }

        public void saveFile() {
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

        public bool verifyOpenedFile() {
            DialogResult result;
            if(editor != null) {
                result = MessageBox.Show("There is an already opened file in the editor, continue?", "Information", MessageBoxButtons.YesNo);

                if(result == DialogResult.Yes) {
                    return true;
                } else {
                    return false;
                }

                return true;
            } else {
                result = DialogResult.None;
                return true;
            }
        }

        public void openAboutDialog() {
            AboutDialog aboutDialog = new AboutDialog();
            aboutDialog.ShowDialog();
        }

        public void unlockToolStripItems() {
            if(editor != null) {
                for(int i = 0; i < toolStrip1.Items.Count; i++) {
                    toolStrip1.Items[i].Enabled = true;
                }
            }
            toolStrip2.Enabled = true;
        }

        public void lockToolStripItems() {
            if(editor == null) {
                // Lock all toolStrip buttons if no file has been opened yet.
                for(int i = 2; i < toolStrip1.Items.Count; i++) {
                    toolStrip1.Items[i].Enabled = false;
                }
            }

            toolStrip2.Enabled = false;
        }

        // Creates everything related to the color palette
        public bool createColorPalette() {
            // Get file 'editor' color set
            // Set color palette controls
            // Show color palette controls
            return true;
        }

        /* ========================================================================================================= */

        private void openToolStripMenuItem_Click(object sender, EventArgs e) {
            openFile();
        }

        private void textBox1_TextChanged(object sender, EventArgs e) {

        }

        private void Main_Load(object sender, EventArgs e) {
            this.Text = ("Vincebus's app test");

            // Behaviour if no document has been opened
            lockToolStripItems();
        }


        private void saveToolStripMenuItem_Click(object sender, EventArgs e) {
            saveFile();
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e) {
            newFile();
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

        private void toolStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e) {

        }

        private void toolStripButton1_Click(object sender, EventArgs e) {
            newFile();
        }

        private void mainPictureBox_Click(object sender, EventArgs e) {

        }

        private void tsButtonCut_Click(object sender, EventArgs e) {

        }

        private void toolStripButton2_Click(object sender, EventArgs e) {

        }

        private void fileToolStripMenuItem_Click(object sender, EventArgs e) {

        }

        private void Main_DpiChanged(object sender, DpiChangedEventArgs e) {

        }

        private void tsButtonOpen_Click(object sender, EventArgs e) {
            openFile();
        }

        private void tsButtonSave_Click(object sender, EventArgs e) {
            saveFile();
        }

        private void mainPictureBox_Paint(object sender, PaintEventArgs e) {
            /*
            Graphics g = e.Graphics;
            g.DrawString("This string follows the cursor!", new Font("Arial", 10), Brushes.Blue, new Point(30, 30));
            g.DrawLine(Pens.Red, mainPictureBox.Left, mainPictureBox.Top, mainPictureBox.Right, mainPictureBox.Bottom);
            */
        }

        private void mainPictureBox_MouseMove(object sender, MouseEventArgs e) {
            /*
            int x = e.X;
            int y = e.Y;

            Graphics g = mainPictureBox.CreateGraphics();
            g.DrawString("Hello World!", new Font("Arial", 12), Brushes.Black, x, y);
            */

            if(e.X < editor.getWidth() && e.Y < editor.getHeight()) {
                statusStrip1.Items["tssMouseCoords"].Text = "(X:" + e.X + "," + "Y:" + e.Y + ")";

            }
           
            switch(this.selectedTool) {
                case 0: {
                        pencilTool(sender, e);
                    }
                    break;
                case 1: {
                        drawRectangleTool(sender, e);
                    }
                    break;

            }
        }

        private void pencilTool(object sender, MouseEventArgs e) {
            if(this.isDrawing == true) {
                using(Graphics g = Graphics.FromImage(mainPictureBox.Image)) {
                    g.DrawLine(Pens.Black, lastPoint, e.Location);

                }
                lastPoint = e.Location;
                mainPictureBox.Invalidate();
            }
        }

        private void drawRectangleTool(object sender, MouseEventArgs e) {
            if(this.isDrawing == true) {
                // Draw on editor layer
                editorLayer = Graphics.FromImage(mainPictureBox.Image);
                editorLayer.DrawRectangle(Pens.Black, new Rectangle(lastPoint, new Size(e.Location.X - lastPoint.X, e.Location.Y - lastPoint.Y)));
                                    
                //lastPoint = e.Location;
                mainPictureBox.Invalidate();
            } else {
                // Draw on bitmap on click release

            }
        }

        private void mainPictureBox_MouseDown(object sender, MouseEventArgs e) {
            this.isDrawing = true;
            this.lastPoint = e.Location;
        }

        private void mainPictureBox_MouseUp(object sender, MouseEventArgs e) {
            this.isDrawing = false;
        }

        private void toolStripContainer1_TopToolStripPanel_Click(object sender, EventArgs e) {

        }

        private void toolStripContainer2_TopToolStripPanel_Click(object sender, EventArgs e) {

        }

        private void toolStripContainer1_BottomToolStripPanel_Click(object sender, EventArgs e) {

        }

        private void tsButtonNewFile_MouseHover(object sender, EventArgs e) {
            tssInfo.Text = "Creates a new file";
        }

        private void tsButtonNewFile_MouseLeave(object sender, EventArgs e) {
            tssInfo.Text = "";
        }

        private void viewToolStripMenuItem_Click(object sender, EventArgs e) {

        }

        private void detectUsedColorsToolStripMenuItem_Click(object sender, EventArgs e) {

        }

        private void toolStrip2_ItemClicked(object sender, ToolStripItemClickedEventArgs e) {

        }

        private void tsButtonPencil_CheckedChanged(object sender, EventArgs e) {

        }

        private void tsButtonPencil_Click(object sender, EventArgs e) {
            foreach(ToolStripButton item in ((ToolStripButton)sender).GetCurrentParent().Items) {
                if(item == sender) {
                    item.Checked = true;
                    selectedTool = 0;
                }
                if((item != null) && (item != sender)) {
                    item.Checked = false;
                }
            }
        }

        private void toolStripButton2_Click_1(object sender, EventArgs e) {
            foreach(ToolStripButton item in ((ToolStripButton)sender).GetCurrentParent().Items) {
                if(item == sender) {
                    item.Checked = true;
                    selectedTool = 1;
                }
                if((item != null) && (item != sender)) {
                    item.Checked = false;

                }
            }
        }

        private void toolStripButton1_Click_1(object sender, EventArgs e) {
            foreach(ToolStripButton item in ((ToolStripButton)sender).GetCurrentParent().Items) {
                if(item == sender) {
                    item.Checked = true;
                    selectedTool = 2;
                }
                if((item != null) && (item != sender)) {
                    item.Checked = false;
                }
            }
        }

        private void toolStripStatusLabel2_Click(object sender, EventArgs e) {

        }
    }
}
