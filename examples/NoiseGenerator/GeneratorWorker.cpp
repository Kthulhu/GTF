//
//  GeneratorWorker.cpp
//  gtf
//
//  Created by David Gallardo on 29/05/16.
//  Copyright (c) 2016 GTF Development Group. All rights reserved.
//

#include "GeneratorWorker.h"
#include "PerlinNoise.h"


#include <cmath>
#include <thread>
#include <mutex>
#include <iostream>

GeneratorWorker::GeneratorWorker()
{
    /*m_gradient.getMarks().clear();
    m_gradient.addMark(0.0f, GTFColor(0xFFA0793D));
    m_gradient.addMark(0.2f, GTFColor(0xFFAA8347));
    m_gradient.addMark(0.3f, GTFColor(0xFFB48D51));
    m_gradient.addMark(0.4f, GTFColor(0xFFBE975B));
    m_gradient.addMark(0.6f, GTFColor(0xFFC8A165));
    m_gradient.addMark(0.7f, GTFColor(0xFFD2AB6F));
    m_gradient.addMark(0.8f, GTFColor(0xFFDCB579));
    m_gradient.addMark(1.0f, GTFColor(0xFFE6BF83));
    */
    //m_gradient.addMark(0.0f, GTFColor(0xFFFFFF00));
    //m_gradient.addMark(0.2f, GTFColor(0xFFFFFF00));
    //m_gradient.addMark(0.3f, GTFColor(0xFF00FF00));
    //m_gradient.addMark(0.4f, GTFColor(0xFF00FFFF));
    //m_gradient.addMark(0.6f, GTFColor(0xFF0000FF));
    //m_gradient.addMark(0.7f, GTFColor(0xFFFF00FF));
    //m_gradient.addMark(1.0f, GTFColor(0xFF0000FF));
    
    m_gradient.getMarks().clear();
    m_gradient.addMark(0.0f, ImColor(0xA0, 0x79, 0x3D));
    m_gradient.addMark(0.2f, ImColor(0xAA, 0x83, 0x47));
    m_gradient.addMark(0.3f, ImColor(0xB4, 0x8D, 0x51));
    m_gradient.addMark(0.4f, ImColor(0xBE, 0x97, 0x5B));
    m_gradient.addMark(0.6f, ImColor(0xC8, 0xA1, 0x65));
    m_gradient.addMark(0.7f, ImColor(0xD2, 0xAB, 0x6F));
    m_gradient.addMark(0.8f, ImColor(0xDC, 0xB5, 0x79));
    m_gradient.addMark(1.0f, ImColor(0xE6, 0xBF, 0x83));

}

bool GeneratorWorker::update(GeneratorInfo& info)
{
    if(info.dirty)
    {
        if(m_running)
        {
            m_cancel = true;
            m_workingThread.join();
            delete [] m_info.image;
        }
        
        m_info = info;
        m_ready = false;
        m_running = true;
        m_cancel = false;
        info.dirty = false;
        m_info.image = new unsigned char[m_info.resX * m_info.resY * 3u];
        m_workingThread = std::thread(&GeneratorWorker::run, this);
    }
    
    if(m_running && m_ready)
    {
        m_workingThread.join();
        m_running = false;
        m_ready = false;
        info.image = m_info.image;
        return true;
    }
    
    return false;
}

void GeneratorWorker::run()
{
    PerlinNoise pn(m_info.seed);

    unsigned num_cpus = std::thread::hardware_concurrency();
    //std::cout << "Launching " << num_cpus << " threads" << std::endl;
    
    std::vector<std::thread> threads(num_cpus);
    unsigned int elementsPerCPU = m_info.resX * m_info.resY / num_cpus;
    
    for (unsigned threadIndex = 0; threadIndex < num_cpus; ++threadIndex)
    {
        threads[threadIndex] = std::thread([=]
        {
            // Visit every pixel of the image and assign a color generated with Perlin noise
            int from = threadIndex * elementsPerCPU;
            int to = (threadIndex + 1) * elementsPerCPU;
            //GTFColor color;
            for(int e = from; e < to; e++)
            {
                int i = e % m_info.resX;
                int j = e / m_info.resY;
                
                if(m_cancel)
                {
                    return;
                }
                
                double x = (double)i/((double)m_info.resX);
                double y = (double)j/((double)m_info.resY);
                
                double n = 0;
                
                if(!m_info.wood)
                {
                    // Typical Perlin noise
                    n = pn.noise(m_info.panX * m_info.density * x, m_info.panY *  m_info.density * y, m_info.panZ);
                }
                else
                {
                    // Wood like structure
                    n = m_info.density * pn.noise(m_info.panX  * x, m_info.panY * y, m_info.panZ);
                    n = n - floor(n);
                }
                
                
                // Map the values to the [0, 255] interval, for simplicity we use
                // 50 shaders of grey
                float color[4];
                m_gradient.getColorAt(float(n), color);
                //color.asU8A(&m_info.image[(e*3)+0]);
                
                
                m_info.image[(e*3)+0] = (unsigned char)floor(255 * color[0]);
                m_info.image[(e*3)+1] = (unsigned char)floor(255 * color[1]);
                m_info.image[(e*3)+2] = (unsigned char)floor(255 * color[2]);
            }
        });
	}
    
    for (auto& t : threads) {
        t.join();
    }
    
    m_ready = true;
}