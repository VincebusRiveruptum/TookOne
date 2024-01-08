using System;
using System.Collections.Generic;
using System.Linq;
using System.Security;
using System.Text;
using System.Threading.Tasks;


namespace animationApp {
    public partial class Main : Form {

        private Editor editor;
        private bool isDrawing;
        private Point lastPoint;
        private Graphics editorLayer;
        private int selectedTool;
        private AppProperties appProperties;

        public Main() {
            InitializeComponent();
            this.isDrawing = false;
            this.lastPoint = new Point();
            this.selectedTool = 0;
            this.appProperties = new AppProperties();
            mainPictureBox.Enabled = false;
        }


        // Setters & getters
        public void SetEditor(Editor editor) {
            this.editor = editor;

            // We update the pictureBox with the new picture created
            mainPictureBox.Image = editor.getBitmap();
        }

        // APP PROPERTIES ===================================================================

        public void setCenteredWorkspace(bool centeredWorkspace) {
            this.appProperties.setCenteredWorkspace(centeredWorkspace);
        }

        public bool getCenteredWorkspace() {
            return this.appProperties.getCenteredWorkspace();
        }

        // ==================================================================================
        private void newFile() {
            // Verify if a file was already created
            if(verifyOpenedFile()) {
                NewFileDialog newFileDialog = new NewFileDialog(this);
                newFileDialog.ShowDialog();
                setWorkspace();
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
                    setWorkspace();
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
            checkUndoRedo();
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

        public void undo() {
            Bitmap prevState = editor.undo();
            if(prevState != null) {
                mainPictureBox.Image = prevState;
            }
        }

        public void redo() {
            Bitmap nextState = editor.redo();
            if(nextState != null) {
                mainPictureBox.Image = nextState;
            }
        }

        public void checkUndoRedo() {
            if(editor == null || editor.isUndoEmpty() == true) {
                toolStrip1.Items["tsButtonUndo"].Enabled = false;
            } else {
                toolStrip1.Items["tsButtonUndo"].Enabled = true;
            }

            if(editor == null || editor.isRedoEmpty() == true) {
                toolStrip1.Items["tsButtonRedo"].Enabled = false;
            } else {
                toolStrip1.Items["tsButtonRedo"].Enabled = true;
            }
        }

        public void setWorkspace() {
            if(editor != null) {
                mainPictureBox.Width = editor.getWidth();
                mainPictureBox.Height = editor.getHeight();
            }
        }

        /* ========================================================================================================= */

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

        public void workspaceCenter() {
            appProperties.setCenteredWorkspace(true);
        }
        public void workspaceCorner() {
            appProperties.setCenteredWorkspace(false);
            mainPictureBox.Location = new Point(5, 5);
        }

        public void updateWorkspace() {
            this.Invalidate();
            this.Refresh();
        }

    }
}
