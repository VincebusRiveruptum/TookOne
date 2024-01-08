using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Drawing.Imaging;
using System.Security.Cryptography;

namespace animationApp {

    public class Editor {
        private Bitmap workspace;
        private Stack<Bitmap> undoStates;
        private Stack<Bitmap> redoStates;

        private int width;
        private int height;
        public Editor(int width, int height, int colorbpp) {
            switch(colorbpp) {
                case 0: this.workspace = new Bitmap(width, height, PixelFormat.Format1bppIndexed);             // Mono
                    break;
                case 1: this.workspace = new Bitmap(width, height, PixelFormat.Format4bppIndexed);             // 16 colors
                    break;
                case 2: this.workspace = new Bitmap(width, height, PixelFormat.Format8bppIndexed);             // 256 colors
                    break;
                case 3: this.workspace = new Bitmap(width, height, PixelFormat.Format16bppRgb555);             // 16 bit color
                    break;
                case 4: this.workspace = new Bitmap(width, height, PixelFormat.Format24bppRgb);                // 24 bit color
                    break;
                default:
                    this.workspace = new Bitmap(width, height, PixelFormat.Format24bppRgb);                    // 24 bit color
                    break;
            }

            fillColor();

            this.undoStates = new Stack<Bitmap>();
            this.redoStates = new Stack<Bitmap>();
                        
            this.width = width;
            this.height = height;
        }
        public Editor(Bitmap bmp) {
            this.workspace = bmp;
            this.width = bmp.Width;
            this.height = bmp.Height;
            this.undoStates = new Stack<Bitmap>();
            this.redoStates = new Stack<Bitmap>();
        }

        public void fillColor() {
            Graphics g = Graphics.FromImage(this.workspace);
            g.Clear(Color.White);
        }

        public Bitmap getBitmap() {
            return this.workspace;
        }

        public Bitmap popUndoState() {
            return this.undoStates.Pop();
        }

        public Bitmap popRedoState() {
            return this.redoStates.Pop();
        }

        public void pushUndoState(Bitmap bmp) {
            if(bmp != null) {
                this.undoStates.Push(bmp);
            }
        }

        public void pushRedoState(Bitmap bmp) {
            if (bmp != null) {
                this.redoStates.Push(bmp);
            }                       
        }

        public Bitmap undo() {
            Bitmap bmp;
            
            if(this.undoStates.Count > 0) {
                bmp = this.undoStates.Pop();
                this.redoStates.Push(bmp);
                return bmp;

            } else {
                return null;
            }
            
        }

        public Bitmap redo() {
            Bitmap bmp;

            if(this.redoStates.Count > 0) { 
                bmp = this.redoStates.Pop();
                this.undoStates.Push(bmp);
                return bmp;
            } else {
                return null;
            }

        }

        public bool isUndoEmpty() {
            if(undoStates.Count == 0) {
                return true;
            } else {
                return false;
            }
        }
        public bool isRedoEmpty() {
            if(redoStates.Count == 0) {
                return true;
            } else {
                return false;
            }
        }

        public int getWidth() { 
            return this.width;
        }
        public int getHeight() {
            return this.height;
        }
        public void setWidth(int width) {

        }
        public void setHeight(int height) {

        }
        public void setColorDepth(char colorbpp) {
           
        }
    }
}
