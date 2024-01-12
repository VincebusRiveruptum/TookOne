using System;
using System.Collections.Generic;
using System.Drawing.Imaging;
using System.Linq;
using System.Linq.Expressions;
using System.Runtime.InteropServices;
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
        
        public void setDockedColors() {
            /*

            // Docked to form main
            DockPanel dockPanel = new DockPanel();
            DockContent dockContent = new DockContent();            ColorPaletteControl paletteControl = new ColorPaletteControl();
  
            dockPanel.Dock = DockStyle.Fill;
            dockPanel.Theme = new VS2015LightTheme();
            this.Controls.Add(dockPanel);


            if(toolStripContainer1 != null) {
                // Docked window
                dockPanel.Controls.Add(paletteControl);

                // Dockable window
                dockContent.Text = "Dockable Window";
                dockContent.Show(dockPanel, DockState.DockRight);
                paletteControl.Dock = DockStyle.Fill;
                dockContent.Controls.Add(toolStripContainer1);
            }
            */
            /*panel2.Controls.Add(new ColorPaletteControl());*/
            int i = 0, jump=0;
            Point ncolorLocation;
            foreach(KnownColor color in Enum.GetValues(typeof(KnownColor))) {
                PictureBox ncolor = new PictureBox();

                ncolor.Width = 16;
                ncolor.Height = 16;

                if(i % 6 ==0 ) {
                    jump++;
                    i = 0;
                }
                
                ncolor.Location = new Point((16*i), (16*jump) - 16);
                ncolor.BackColor = Color.FromKnownColor(color);
                pAvailableColors.Controls.Add(ncolor);

                ncolor.MouseDown += new MouseEventHandler(nColor_MouseDown);

                i++;
                    
            }
        }
        private void nColor_MouseDown(object sender, MouseEventArgs e) {
            
            PictureBox pbColorN = sender as PictureBox;

            if(e.Button == MouseButtons.Left) {
                appProperties.setSelectedColor(pbColorN.BackColor);
                pbSelectedColor.BackColor = pbColorN.BackColor;
            }

            if(e.Button == MouseButtons.Right) {
                appProperties.setSecondaryColor(pbColorN.BackColor);
                pbSecondaryColor.BackColor = pbColorN.BackColor;
            }
        }

        // Setters & getters
        public void SetEditor(Editor editor) {
            this.editor = editor;
        }

        // APP PROPERTIES ===================================================================

        public void setCenteredWorkspace(bool centeredWorkspace) {
            if(appProperties != null) {
                this.appProperties.setCenteredWorkspace(centeredWorkspace);
            }
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
                setDockedColors();
                

            }
        }

        /* ========================================================================================================= */

        public void bucketTool(object sender, MouseEventArgs e, Color color) {
            // Flood fill region directly to workspace 'editor', thanks stack overflow
            int x = e.X;
            int y = e.Y;

            BitmapData data = editor.getBitmap().LockBits(
                 new Rectangle(0, 0, editor.getWidth(), editor.getHeight()),
                 ImageLockMode.ReadWrite, PixelFormat.Format32bppArgb);
            int[] bits = new int[data.Stride / 4 * data.Height];
            Marshal.Copy(data.Scan0, bits, 0, bits.Length);

            LinkedList<Point> check = new LinkedList<Point>();
            int floodTo = color.ToArgb();
            int floodFrom = bits[x + y * data.Stride / 4];
            bits[x + y * data.Stride / 4] = floodTo;

            if(floodFrom != floodTo) {
                check.AddLast(new Point(x, y));
                while(check.Count > 0) {
                    Point cur = check.First.Value;
                    check.RemoveFirst();

                    foreach(Point off in new Point[] {
                new Point(0, -1), new Point(0, 1),
                new Point(-1, 0), new Point(1, 0)}) {
                        Point next = new Point(cur.X + off.X, cur.Y + off.Y);
                        if(next.X >= 0 && next.Y >= 0 &&
                            next.X < data.Width &&
                            next.Y < data.Height) {
                            if(bits[next.X + next.Y * data.Stride / 4] == floodFrom) {
                                check.AddLast(next);
                                bits[next.X + next.Y * data.Stride / 4] = floodTo;
                            }
                        }
                    }
                }
            }

            Marshal.Copy(bits, 0, data.Scan0, bits.Length);
            editor.getBitmap().UnlockBits(data);
            mainPictureBox.Image = editor.getBitmap();

        }

        private void pencilTool(object sender, MouseEventArgs e) {
            /*
            if(this.isDrawing == true) {
                Pen newPen = new Pen(appProperties.getSelectedColor(), 1);
                //Draw pixel on editor layer
                editorLayer = Graphics.FromImage(pbEditorLayer.Image);
                editorLayer.DrawLine(newPen, initialPoint, e.Location);

                initialPoint = e.Location;
                pbEditorLayer.Invalidate();
            }
            */
            if(this.isDrawing == true) {

                //We translate the pixel with relation to the zoom value
                Point putPixelCoords = new Point((int)(e.Location.X / appProperties.getZoomScaleSelectedValue()), (int)(e.Location.Y / appProperties.getZoomScaleSelectedValue()));
                Point translatedInitialPoint = new Point((int)(initialPoint.X / appProperties.getZoomScaleSelectedValue()),(int)(initialPoint.Y / appProperties.getZoomScaleSelectedValue()));
                

                Pen newPen = new Pen(appProperties.getSelectedColor(), 1);
                //Draw pixel on editor workspace layer
                editorLayer = Graphics.FromImage(editor.getBitmap());
                editorLayer.DrawLine(newPen, translatedInitialPoint, putPixelCoords);

                editorLayer = Graphics.FromImage(mainPictureBox.Image);
                editorLayer.DrawImage(editor.getBitmap(), new Rectangle(0, 0, pbEditorLayer.Width, pbEditorLayer.Height));

                initialPoint = e.Location;
                pbEditorLayer.Invalidate();
            }
        }
        public void drawLineTool(object sender, MouseEventArgs e) {

            // Line is being drawn only if the mouse button is being hold down
            if(this.isDrawing == true) {
                // Clear editorLayer
                editorLayer = Graphics.FromImage(pbEditorLayer.Image);
                editorLayer.Clear(Color.Transparent);

                // Draw Line
                editorLayer.DrawLine(Pens.Black, initialPoint, e.Location);
                pbEditorLayer.Invalidate();
            }
        }


        public void drawLineToWorkspace(object sender, MouseEventArgs e) {
            mainPictureBox.Image = this.overwriteImages(mainPictureBox.Image, pbEditorLayer.Image, Color.White);
            editor.setBitmap((Bitmap)mainPictureBox.Image);
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
                editorLayer.DrawRectangle(appProperties.getBrushTool(), new Rectangle(initialPoint.X , initialPoint.Y, e.Location.X - initialPoint.X, e.Location.Y - initialPoint.Y));

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

            // If there is no zoom then we save the already done changes to editor buffer
            if(appProperties.getZoomScalePosition() == 2) {
                editor.setBitmap((Bitmap)mainPictureBox.Image);

            }

            // Clear editor layer buffer
            editorLayer = Graphics.FromImage(pbEditorLayer.Image);
            editorLayer.Clear(Color.Transparent);

        }

        // This functions draw the rectangle when the user released the mouse on the workspace
        public void drawRectangleToWorkspace(object sender, MouseEventArgs e) {
            // Draw on editor layer

            editorLayer = Graphics.FromImage(mainPictureBox.Image);
            editorLayer.DrawRectangle(appProperties.getBrushTool(), new Rectangle(initialPoint, new Size(e.Location.X - initialPoint.X, e.Location.Y - initialPoint.Y)));
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

        // Zoom methods ===================================================

        // Zoom out the workspace and editor layer
        public void zoomOut() {
            Bitmap scaledImage;
            int scaledWidth = 0, scaledHeight = 0;

            // We decrease de zoom scale
            if(appProperties.getZoomScalePosition() > 0) {
                appProperties.setZoomScalePosition(appProperties.getZoomScalePosition() - 1);
            }
            statusStrip1.Items["tssZoomInfo"].Text = "" + appProperties.getZoomScale(appProperties.getZoomScalePosition()) + "x";

            scaledWidth = (int)(editor.getWidth() * appProperties.getZoomScale(appProperties.getZoomScalePosition()));
            scaledHeight = (int)(editor.getHeight() * appProperties.getZoomScale(appProperties.getZoomScalePosition()));

            scaledImage = new Bitmap(scaledWidth, scaledHeight);

            // Zoom editor layer

            pbEditorLayer.Width = scaledWidth;
            pbEditorLayer.Height = scaledHeight;

            pbEditorLayer.Image = new Bitmap(scaledWidth, scaledHeight);

            // Zoom workspace page

            mainPictureBox.Width = scaledWidth;
            mainPictureBox.Height = scaledHeight;

            editorLayer = Graphics.FromImage(scaledImage);
            editorLayer.DrawImage(editor.getBitmap(), new Rectangle(0, 0, scaledWidth, scaledHeight));

            mainPictureBox.Image = scaledImage;


        }

        // Zoom in the workspace and editor layer
        public void zoomIn() {
            Bitmap scaledImage;
            int scaledWidth = 0, scaledHeight = 0;

            statusStrip1.Items["tssZoomInfo"].Text = "" + appProperties.getZoomScale(appProperties.getZoomScalePosition()) + "x";
            
            // We decrease de zoom scale
            if(appProperties.getZoomScalePosition() < appProperties.getZoomScalesLength()) {
                appProperties.setZoomScalePosition(appProperties.getZoomScalePosition() + 1);
            }

            scaledWidth = (int)(editor.getWidth() * appProperties.getZoomScale(appProperties.getZoomScalePosition()));
            scaledHeight = (int)(editor.getHeight() * appProperties.getZoomScale(appProperties.getZoomScalePosition()));

            scaledImage = new Bitmap(scaledWidth, scaledHeight);

            // Zoom editor layer

            pbEditorLayer.Width = scaledWidth;
            pbEditorLayer.Height = scaledHeight;

            pbEditorLayer.Image = new Bitmap(scaledWidth, scaledHeight);
            

            statusStrip1.Items["tssZoomInfo"].Text = "" + pbEditorLayer.Width + "," + pbEditorLayer.Height;

            // Zoom workspace page

            mainPictureBox.Width = scaledWidth;
            mainPictureBox.Height = scaledHeight;

            editorLayer = Graphics.FromImage(scaledImage);
            editorLayer.DrawImage(editor.getBitmap(), new Rectangle(0, 0, scaledWidth, scaledHeight));

            mainPictureBox.Image = scaledImage;
        }


    }
}
