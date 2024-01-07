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

            this.width = width;
            this.height = height;
        }
        public Editor(Bitmap bmp) {
            this.workspace = bmp;
            this.width = bmp.Width;
            this.height = bmp.Height;
        }

        public void fillColor() {
            Graphics g = Graphics.FromImage(this.workspace);
            g.Clear(Color.White);
        }

        public Bitmap getBitmap() {
            return this.workspace;
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
