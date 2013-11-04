
/*!  \brief  ProtoVerletSurface.h: Verlet surface implementation
 Protobyte Library v02
 
 Created by Ira on 6/5/13.
 Copyright (c) 2013 Ira Greenberg. All rights reserved.
 
 Library Usage:
 This work is licensed under the Creative Commons
 Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 To view a copy of this license, visit
 http://creativecommons.org/licenses/by-nc-sa/3.0/
 or send a letter to Creative Commons,
 444 Castro Street, Suite 900,
 Mountain View, California, 94041, USA.
 
 This notice must be retained any source distribution.
 
 \ingroup common
 This class is part of the group common (update)
 \sa NO LINK
 */

#include "ProtoVerletSurface.h"


using namespace ijg;


ProtoVerletSurface::ProtoVerletSurface(){
    
}

ProtoVerletSurface::ProtoVerletSurface(const Vec3f& pos, const Vec3f& rot, const ProtoDimension3<float>& size, const ProtoColor4<float>& col4, int rowCount, int columnCount, float tension, AnchorModeEnum anchorMode):
ProtoGeom3(pos, rot, size, col4), rowCount(rowCount), columnCount(columnCount), tension(tension), anchorMode(anchorMode)
{
    // ensure even cols and rows
    rowCount = (rowCount%2!=0) ? rowCount+1 : rowCount;
    columnCount = (columnCount%2!=0) ? columnCount+1 : columnCount;
    
    centroidIndex = (rowCount/2-1)*(columnCount-1) + (columnCount-1)/2;
    pulseTheta = 0;
    init(); // calls calcVertices/calcIndices/calcFaces/etc
}

ProtoVerletSurface::ProtoVerletSurface(const Vec3f& pos, const Vec3f& rot, const ProtoDimension3<float>& size, const ProtoColor4<float>& col4, int rowCount, int columnCount, float tension, std::string imageMap, AnchorModeEnum anchorMode):
ProtoGeom3(pos, rot, size, col4), rowCount(rowCount), columnCount(columnCount), tension(tension), imageMap(imageMap), anchorMode(anchorMode)
{
    
    // ensure even cols and rows
    rowCount = (rowCount%2!=0) ? rowCount+1 : rowCount;
    columnCount = (columnCount%2!=0) ? columnCount+1 : columnCount;
    std::cout << "rowCount = " << rowCount << std::endl;
    std::cout << "columnCount = " << columnCount << std::endl;
    
    // hard code texture for testing
    texture = ProtoTexture2(imageMap, 300, 300, 0);
    GLuint texID = texture.getTextureID();
    
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, texID);
    
    
    centroidIndex = (rowCount/2-1)*(columnCount-1) + (columnCount-1)/2;
    pulseTheta = 0;
    init(); // calls calcVertices/calcIndices/calcFaces/etc
}

// calculate verlet geoemetry
void ProtoVerletSurface::calcVerts(){
    
    // set mesh color
    
    meshColor = col4;
    // verlet sheet
    //ind = (rows-2)/2*(cols) + cols/2;
    std::cout << " size = " << size << std::endl;
    float cellW = size.w/(columnCount-1);
    float cellH = size.h/(rowCount-1);
    for(int i=0; i<rowCount; ++i){
        for(int j=0; j<columnCount; ++j){
            float x = -size.w/2 + cellW*j;
            float y = size.h/2 - cellH*i;
            float z = 0;//ProtoMath::random(-.02, .02);
            balls.push_back(std::shared_ptr<ProtoVerletBall>(new ProtoVerletBall(Vec3f(x, y, z))));
            verts.push_back(ProtoVertex3(Vec3f(x, y, z),
                                         ProtoColor4f(col4.getR(), col4.getG(), col4.getB(), col4.getA()), ProtoTuple2f(cellW/size.w*j, cellH/size.h*i)));
            
        }
    }
    
    //centroidIndex = (static_cast<int>(balls.size())-1)/2+columnCount/2;
    std::cout << "balls.size() = " << balls.size()<< std::endl;
    std::cout << "centroidIndex = " << centroidIndex << std::endl;
    
    for(int i=0, k=0, l=0, m=0, n=0; i<rowCount-1; ++i){
        for(int j=0; j<columnCount-1; ++j){
            k = i*columnCount+j;
            l = (i+1)*columnCount+j;
            m = (i+1)*columnCount+j+1;
            n = i*columnCount+j+1;
            switch(anchorMode){
                    
                    // ALL CORNERS
                case ALL_CORNERS:
                    // TL
                    if (i==0 && j==0){
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(l), tension, Tup2f(0, 1))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(l), balls.at(m), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(m), balls.at(n), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(n), balls.at(k), tension, Tup2f(1, 0))));
                        // diag
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(m), tension, Tup2f(0, 1))));
                        //sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(m), tension, Tup2f(.5, .5))));
                        // TR
                    } else  if (i==0 && j==columnCount-2){
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(l), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(l), balls.at(m), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(m), balls.at(n), tension, Tup2f(1, 0))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(n), balls.at(k), tension, Tup2f(0, 1))));
                        // diag
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(m), tension, Tup2f(.5, .5))));
                        // BR
                    }  else  if (i==rowCount-2 && j==columnCount-2){
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(l), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(l), balls.at(m), tension, Tup2f(1, 0))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(m), balls.at(n), tension, Tup2f(0, 1))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(n), balls.at(k), tension, Tup2f(.5, .5))));
                        // diag
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(m), tension, Tup2f(1, 0))));
                        //sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(m), tension, Tup2f(.5, .5))));
                        //BL
                    } else  if (i==rowCount-2 && j==0){
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(l), tension, Tup2f(1, 0))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(l), balls.at(m), tension, Tup2f(0, 1))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(m), balls.at(n), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(n), balls.at(k), tension, Tup2f(.5, .5))));
                        // diag
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(m), tension, Tup2f(.5, .5))));
                    } else {
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(l), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(l), balls.at(m), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(m), balls.at(n), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(n), balls.at(k), tension, Tup2f(.5, .5))));
                        // diag
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(m), tension, Tup2f(.5, .5))));
                    }
                    break;
                    
                    // ALL EDGES
                case ALL_EDGES:
                    // Top
                    if (i==0){
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(l), tension, Tup2f(0, 1))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(l), balls.at(m), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(m), balls.at(n), tension, Tup2f(1, 0))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(n), balls.at(k), tension, Tup2f(0, 0))));
                        // diag
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(m), tension, Tup2f(0, 1))));
                        //sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(m), tension, Tup2f(.5, .5))));
                        // Right
                    } else  if (i > 0 && j==columnCount-2){
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(l), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(l), balls.at(m), tension, Tup2f(1, 0))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(m), balls.at(n), tension, Tup2f(0, 0))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(n), balls.at(k), tension, Tup2f(0, 1))));
                        // diag
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(m), tension, Tup2f(1, 0))));
                        // LEFT
                    }  else  if (j==0){
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(l), tension, Tup2f(0, 0))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(l), balls.at(m), tension, Tup2f(0, 1))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(m), balls.at(n), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(n), balls.at(k), tension, Tup2f(1, 0))));
                        // diag
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(m), tension, Tup2f(0, 1))));
                        //sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(m), tension, Tup2f(.5, .5))));
                        //BOTTOM
                    } else  if (i==rowCount-2 && j>0 && j<columnCount-2){
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(l), tension, Tup2f(1, 0))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(l), balls.at(m), tension, Tup2f(0, 0))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(m), balls.at(n), tension, Tup2f(0, 1))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(n), balls.at(k), tension, Tup2f(.5, .5))));
                        // diag
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(m), tension, Tup2f(1, 0))));
                    } else {
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(l), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(l), balls.at(m), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(m), balls.at(n), tension, Tup2f(.5, .5))));
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(n), balls.at(k), tension, Tup2f(.5, .5))));
                        // diag
                        sticks.push_back(std::unique_ptr<ProtoVerletStick>(new ProtoVerletStick(balls.at(k), balls.at(m), tension, Tup2f(.5, .5))));
                    }
                    break;
            }
            
        }
    }
    
}


void ProtoVerletSurface::calcInds(){
    for(int i=0, k=0, l=0, m=0, n=0; i<rowCount-1; ++i){
        for(int j=0; j<columnCount-1; ++j){
            k = i*columnCount+j;
            l = (i+1)*columnCount+j;
            m = (i+1)*columnCount+j+1;
            n = i*columnCount+j+1;
            inds.push_back(ProtoTuple3i(k, l, m));
            inds.push_back(ProtoTuple3i(k, m, n));
        }
    }
    
    
}

void ProtoVerletSurface::nudge(int index){
    balls.at(index)->pos.z += 1.5;
}

// animate VBO data based on Verlet Integration
// TO DO: (MAYBE) move this to GPU with Vertex Shader


// interleavedPrims stride = 12;
void ProtoVerletSurface::flow() {
    
    // packed vertex data
    // stride is 12 : (x, y, z, nx, ny, nz, r, g, b, a, u, v)
    int stride = 12;
    int colOffset = 6;
    // Animate VBO data
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    for (int i = 0; i < inds.size(); ++i) {
        
        // verts
        interleavedPrims.at(inds.at(i).elem0 * stride) = balls.at(inds.at(i).elem0)->pos.x;
        interleavedPrims.at(inds.at(i).elem0 * stride + 1) = balls.at(inds.at(i).elem0)->pos.y;
        interleavedPrims.at(inds.at(i).elem0 * stride + 2) = balls.at(inds.at(i).elem0)->pos.z;
        
        interleavedPrims.at(inds.at(i).elem1 * stride) = balls.at(inds.at(i).elem1)->pos.x;
        interleavedPrims.at(inds.at(i).elem1 * stride + 1) = balls.at(inds.at(i).elem1)->pos.y;
        interleavedPrims.at(inds.at(i).elem1 * stride + 2) = balls.at(inds.at(i).elem1)->pos.z;
        
        interleavedPrims.at(inds.at(i).elem2 * stride) = balls.at(inds.at(i).elem2)->pos.x;
        interleavedPrims.at(inds.at(i).elem2 * stride + 1) = balls.at(inds.at(i).elem2)->pos.y;
        interleavedPrims.at(inds.at(i).elem2 * stride + 2) = balls.at(inds.at(i).elem2)->pos.z;
        
        //mesh cols
        //        interleavedPrims.at(inds.at(i).elem0 * stride + colOffset) = meshColor.getR();
        //        interleavedPrims.at(inds.at(i).elem0 * stride + colOffset + 1) = meshColor.getG();
        //        interleavedPrims.at(inds.at(i).elem0 * stride + colOffset + 2) = meshColor.getB();
        //        interleavedPrims.at(inds.at(i).elem0 * stride + colOffset + 3) = meshColor.getA();
        //
        //        interleavedPrims.at(inds.at(i).elem1 * stride + colOffset) = meshColor.getR();
        //        interleavedPrims.at(inds.at(i).elem1 * stride + colOffset + 1) = meshColor.getG();
        //        interleavedPrims.at(inds.at(i).elem1 * stride + colOffset + 2) = meshColor.getB();
        //        interleavedPrims.at(inds.at(i).elem1 * stride + colOffset + 3) = meshColor.getA();
        //
        //        interleavedPrims.at(inds.at(i).elem2 * stride + colOffset) = meshColor.getR();
        //        interleavedPrims.at(inds.at(i).elem2 * stride + colOffset + 1) = meshColor.getG();
        //        interleavedPrims.at(inds.at(i).elem2 * stride + colOffset + 2) = meshColor.getB();
        //        interleavedPrims.at(inds.at(i).elem2 * stride + colOffset + 3) = meshColor.getA();
        
        
        // TO DO: (MAYBE) Fix Vertex Normals
        //vnorms
        
        
    }
    int vertsDataSize = sizeof (float) * static_cast<int>(interleavedPrims.size());
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertsDataSize, &interleavedPrims[0]); // upload the data
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Activate Verlet/constraints
    for (int i = 0; i < balls.size(); ++i) {
        balls.at(i)->verlet();
    }
    
    
    for (int i = 0; i < sticks.size(); ++i) {
        sticks.at(i)->constrainLen();
    }
    
    // pulse surface
    //balls.at(centroidIndex)->pos += Vec3f(-sin(pulseTheta)*.025, cos(pulseTheta)*.02, sin(pulseTheta)*12.3);
    int colMax = (columnCount-1)/2;
    int rowMax = (rowCount-1)/2;
    int randIndex = static_cast<int>(centroidIndex + columnCount * static_cast<int>(ProtoMath::random(-colMax,colMax))+ static_cast<int>(ProtoMath::random(-rowMax,rowMax)));
    balls.at(randIndex)->pos.z += ProtoMath::random(-6, 6);
   // balls.at(centroidIndex)->pos.z += ProtoMath::random(-6, 6);
    
    pulseTheta += 10*ProtoMath::PI/180.0;
    
}

void ProtoVerletSurface::setMeshColor(const Col4f& meshColor){
    this->meshColor = meshColor;
    
    // packed vertex data
    // stride is 12 : (x, y, z, nx, ny, nz, r, g, b, a, u, v)
    int stride = 12;
    int colOffset = 6;
    // Animate VBO data
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    for (int i = 0; i < inds.size(); ++i) {
        
        
        //mesh cols
        interleavedPrims.at(inds.at(i).elem0 * stride + colOffset) = meshColor.getR();
        interleavedPrims.at(inds.at(i).elem0 * stride + colOffset + 1) = meshColor.getG();
        interleavedPrims.at(inds.at(i).elem0 * stride + colOffset + 2) = meshColor.getB();
        interleavedPrims.at(inds.at(i).elem0 * stride + colOffset + 3) = meshColor.getA();
        
        interleavedPrims.at(inds.at(i).elem1 * stride + colOffset) = meshColor.getR();
        interleavedPrims.at(inds.at(i).elem1 * stride + colOffset + 1) = meshColor.getG();
        interleavedPrims.at(inds.at(i).elem1 * stride + colOffset + 2) = meshColor.getB();
        interleavedPrims.at(inds.at(i).elem1 * stride + colOffset + 3) = meshColor.getA();
        
        interleavedPrims.at(inds.at(i).elem2 * stride + colOffset) = meshColor.getR();
        interleavedPrims.at(inds.at(i).elem2 * stride + colOffset + 1) = meshColor.getG();
        interleavedPrims.at(inds.at(i).elem2 * stride + colOffset + 2) = meshColor.getB();
        interleavedPrims.at(inds.at(i).elem2 * stride + colOffset + 3) = meshColor.getA();
        
        
        // TO DO: (MAYBE) Fix Vertex Normals
        //vnorms
        
        
    }
    int vertsDataSize = sizeof (float) * static_cast<int>(interleavedPrims.size());
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertsDataSize, &interleavedPrims[0]); // upload the data
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
}
