
#include <dlib/array2d.h>
#include <dlib/image_processing.h>
#include <dlib/image_transforms.h>
#include <dlib/image_keypoint.h>
#include <dlib/image_io.h>

using namespace dlib;
using namespace std;

void process(const char *fimg);

template <typename image_type>
void findrect(const image_type &img);

int main(int argc, char *argv[])
{
    process("./test.jpg");

    return 0;
}

template <typename image_type>
void findrect(const image_type &img)
{
    unsigned char k = 6;
    unsigned char maxpix = 255;
    unsigned char minpix = 255 - k;
    bool start = false;
    unsigned int counter = 0, sy, cy, ty;
    std::vector<std::vector<long>> points;

    //
    for(int i = 0; i < img.nr(); i+=2)
    {
        unsigned int pix = 0;
        for(int j = 0; j < img.nc(); j++)
        {
            pix += img[i][j];
        }
        pix = pix / img.nc();

        if(pix >= minpix && pix <= maxpix) // начало межстрочного интервала
        {
            if(!start)
            {
                sy = i;
                start = true;
            }
            //qDebug() << i << " W:" << pix;
        }
        else if(pix < minpix) // начало текста
        {
            if(start)
            {
                counter++;
                cy = (sy + i ) / 2; // середина межстрочного интервала
                start = false;

                if(counter > 1)
                {
                    unsigned int by = cy;
                    points.push_back({ty, by});
                }
                ty = cy;
            }
            //qDebug() << i << " B:" << pix;
        }
    }
    points.push_back({ty, img.nr()});

    //qDebug() << "lines.size()" << points.size();

    for(size_t i = 0; i < points.size(); i++)
    {
        array2d<unsigned char> tmp;
        tmp.set_size(points[i][1] - points[i][0], img.nc());
        long tr = 0;
        for(long r = points[i][0]; r < img.nr() && tr < tmp.nr(); r++, tr++)
           {
               for(long c = 0; c < img.nc(); c++)
               {
                   assign_pixel(tmp[tr][c], img[r][c]);
               }
           }

        //qDebug() << "rect()" << points[i][0] << points[i][1];
	std::string fn = "./test-";
        save_jpeg(tmp, fn.append(std::to_string(i+1)).append(".jpg").c_str());
    }
}


//
void process(const char *fimg)
{
    dlib::array2d<dlib::rgb_pixel> img;
    dlib::load_image(img, fimg);

    //qDebug() << "img.size()" << img.nr() << img.nc();

    dlib::array2d<unsigned char> out, out1, out2;

    // 1
    typename pixel_traits<typename image_traits<array2d<dlib::rgb_pixel>>::pixel_type>::basic_pixel_type thresh = 67;
    threshold_image(img, out1, thresh);

    // 2
    typename pixel_traits<typename image_traits<array2d<dlib::rgb_pixel>>::pixel_type>::basic_pixel_type lower_thresh = 60;
    typename pixel_traits<typename image_traits<array2d<dlib::rgb_pixel>>::pixel_type>::basic_pixel_type upper_thresh = 75;
    hysteresis_threshold(img, out2, lower_thresh, upper_thresh);

    // 3
    binary_union(out1, out2, out);

    findrect(out);

    dlib::save_jpeg(out, "./test-new.jpg");
}




