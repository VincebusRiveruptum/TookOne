using System;
using System.Collections.Generic;
using System.Drawing.Imaging;
using System.Linq;
using System.Linq.Expressions;
using System.Security;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;


namespace animationApp {
    public partial class Main : Form {

        private Editor editor;
        private bool isDrawing;
        private Point initialPoint;
        private Graphics editorLayer;
        private int selectedTool;
        private AppProperties appProperties;

        public Main() {
            InitializeComponent();
            this.isDrawing = false;
            this.initialPoint = new Point();
            this.selectedTool = 0;
            this.appProperties = new AppProperties();
            
            mainPictureBox.Enabled = false;
            pbEditorLayer.Enabled = false;
        }


        // Setters & getters
        public void SetEditor(Editor editor) {
            this.editor = editor;
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

                mainPictureBox.Image = editor.getBitmap();

                pbEditorLayer.Image = new Bitmap(editor.getWidth(), editor.getHeight());

                pbEditorLayer.Location = new Point(0,0);
                pbEditorLayer.Width = mainPictureBox.Width;
                pbEditorLayer.Height = mainPictureBox.Height;
                pbEditorLayer.Parent = mainPictureBox;
                pbEditorLayer.Enabled = true;

                appProperties.resetProperties();
            }
        }

        /* ========================================================================================================= */

        private void pencilTool(object sender, MouseEventArgs e) {

            if(this.isDrawing == true) {

                //Draw pixel on editor layer
                editorLayer = Graphics.FromImage(pbEditorLayer.Image);
                editorLayer.DrawLine(Pens.Black, initialPoint, e.Location);

                initialPoint = e.Location;
                pbEditorLayer.Invalidate();
            }
        }

        private void drawRectangleTool(object sender, MouseEventArgs e) {
            if(this.isDrawing == true) {

                Rectangle rect;
                // Clear pbEditorLayer

                editorLayer = Graphics.FromImage(pbEditorLayer.Image);
                editorLayer.Clear(Color.Transparent);


                // Draw on editor layer
                
                editorLayer = Graphics.FromImage(pbEditorLayer.Image);

                // Setting origin to 0 half of the height and weidth, so we can translate editor layer coordinates for negative drawing of shapes
                editorLayer.TranslateTransform(pbEditorLayer.Width / 2 , pbEditorLayer.Height / 2);

                //editorLayer.DrawRectangle(Pens.Black, new Rectangle(initialPoint.X - (pbEditorLayer.Width / 2), initialPoint.Y - (pbEditorLayer.Height / 2), e.Location.X, e.Location.Y));
                editorLayer.DrawRectangle(Pens.Black, new Rectangle(initialPoint.X , initialPoint.Y, e.Location.X - initialPoint.X, e.Location.Y - initialPoint.Y));

                statusStrip1.Items["tssMouseCoords"].Text = " Rectangle Coords [X: " + translateX(e.Location.X) + ", Y: " + translateY(e.Location.Y) + "]";

                //lastPoint = e.Location;
                pbEditorLayer.Invalidate();
                    
            } else {
                // Draw on bitmap on click release

            }
            
        }

        public float translateX(int x) {
            return (float)(x - (pbEditorLayer.Width / 2));
        }

        public float translateY(int y) {
            return (float)(y - (pbEditorLayer.Height / 2));
        }

        // This function transfer the editorLayer buffer to the 'workspace' or 'editor' buffer
        public void transferToWorkspace() {
            // Overlap editor layer newly made drawing to main workspace
            mainPictureBox.Image = this.overwriteImages(mainPictureBox.Image, pbEditorLayer.Image, Color.White);
            editor.setBitmap((Bitmap)mainPictureBox.Image);

            // Clear editor buffer
            editorLayer = Graphics.FromImage(pbEditorLayer.Image);
            editorLayer.Clear(Color.Transparent);

        }

        // This functions draw the rectangle when the user released the mouse on the workspace
        public void drawRectangleToWorkspace(object sender, MouseEventArgs e) {
            // Draw on editor layer

            editorLayer = Graphics.FromImage(mainPictureBox.Image);
            editorLayer.DrawRectangle(Pens.Black, new Rectangle(initialPoint, new Size(e.Location.X - initialPoint.X, e.Location.Y - initialPoint.Y)));
            //mainPictureBox.Invalidate();
        }

        public void workspaceCenter() {
            this.appProperties.setCenteredWorkspace(true);
        }
        public void workspaceCorner() {
            this.appProperties.setCenteredWorkspace(false);
            mainPictureBox.Location = new Point(5, 5);
        }

        public void updateWorkspace() {
            this.Invalidate();
            this.Refresh();
        }

        public Bitmap blendImages(Image image1, Image image2, Color transparentColor, float opacity) {
            Bitmap bitmap1 = new Bitmap(image1);
            Bitmap bitmap2 = new Bitmap(image2);

            bitmap1.MakeTransparent(transparentColor);

            using(Graphics g = Graphics.FromImage(bitmap1)) {
                float[][] matrixItems =
                {
                    new float[] {1, 0, 0, 0, 0},
                    new float[] {0, 1, 0, 0, 0},
                    new float[] {0, 0, 1, 0, 0},
                    new float[] {0, 0, 0, opacity, 0},
                    new float[] {0, 0, 0, 0, 1}
                  };

                ColorMatrix colorMatrix = new ColorMatrix(matrixItems);
                ImageAttributes imageAttributes = new ImageAttributes();
                imageAttributes.SetColorMatrix(colorMatrix, ColorMatrixFlag.Default, ColorAdjustType.Bitmap);

                g.DrawImage(bitmap2, new Rectangle(0, 0, bitmap1.Width, bitmap1.Height), 0, 0, bitmap2.Width, bitmap2.Height, GraphicsUnit.Pixel, imageAttributes);
            }

            return bitmap1;
        }

        private Bitmap overwriteImages(Image dest, Image src, Color transparentColor) {
            Bitmap bitmap1 = new Bitmap(dest);
            Bitmap bitmap2 = new Bitmap(src);

            //bitmap1.MakeTransparent(transparentColor);
            bitmap2.MakeTransparent(transparentColor);

            using(Graphics g = Graphics.FromImage(bitmap1)) {
                g.DrawImage(bitmap2, 0, 0);
            }

            return bitmap1;
        }

    }
}
