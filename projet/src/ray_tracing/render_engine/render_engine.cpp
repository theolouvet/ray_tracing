/*
**    TP CPE Lyon
**    Copyright (C) 2015 Damien Rohmer
**
**    This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**   This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "render_engine.hpp"

#include "image/image.hpp"
#include "ray_tracing/scene/scene_parameter.hpp"
#include "ray_tracing/scene/ray.hpp"
#include "ray_tracing/primitives/intersection_data.hpp"
#include "ray_tracing/scene/anti_aliasing_table.hpp"
#include <limits>
#include <cmath>

namespace cpe
{


void render(image& im,scene_parameter const& scene)
{
    // **************************************************************************** //
    //
    // Current Algorithm:
    //
    // For all pixels of the image
    //    Generate ray from the camera toward in the direction of the current pixel
    //    Compute associated color (ray tracing algorithm)
    //    Set the color to the current pixel
    //
    // **************************************************************************** //

    camera const& cam = scene.get_camera();

    int const Nx = im.Nx();
    int const Ny = im.Ny();
    int const N = 10;
    anti_aliasing_table table(N);

    /*
     * //loop over (u,v) in [0,1]
    {
        //init value = 0
        for(int dx=0 ; dx<N_sample ; ++dx)
        {
            for(int dy=0 ; dy<N_sample ; ++dy)
            {
                float const du = aa.displacement(dx)/(Nx-1); //Nx is the size in pixel in x direction
                float const dv = aa.displacement(dy)/(Ny-1); //Ny is the size in pixel in y direction

                float const w = aa.weight(dx,dy);

                value += w*F(u+du,v+dv); //F is a function of (u,v)
            }
        }

        //value contains the gaussian weighted average around (u,v) position
        // ...
    }
*/
    // loop over all the pixels of the image
    ray r;
    color a;
    color c = color(0,0,0);
    for(int kx=0 ; kx<Nx ; ++kx)
    {
        float const u = static_cast<float>(kx)/(Nx-1);
        for(int ky=0 ; ky<Ny ; ++ky)
        {
            float const v = static_cast<float>(ky)/(Ny-1);
            // generate ray and compute color

            for(int dx=0 ; dx<N ; ++dx)
            {
                for(int dy=0 ; dy<N ; ++dy)
                {
                    float const du = table.displacement(dx)/(Nx-1); //Nx is the size in pixel in x direction
                    float const dv = table.displacement(dy)/(Ny-1); //Ny is the size in pixel in y direction

                    float const w = table.weight(dx,dy);
                    r = ray_generator(cam,u + du,v + dv);
                    a = ray_trace(r,scene);
                    c += w*a; //F is a function of (u,v)
                    //std::cout << dx << " "<<dy<<" "<< c<<" "<<w<<std::endl;
                }
            }
            im({kx,ky}) = c;
            c = color(0,0,0);
/*
              ray const r = ray_generator(cam,u,v);
                color const c = ray_trace(r,scene);
                im({kx,ky}) = c;
                */

        }
    }

}


ray ray_generator(camera const& cam,float const u,float const v)
{
    // position of the sample on the screen in 3D
    vec3 const p_screen = screen_position(cam,u,v);

    // vector "camera center" to "screen position"
    vec3 const d = p_screen-cam.center();

    // compute the ray
    ray const r(cam.center(),d);

    return r;
}

color ray_trace(ray const& r,scene_parameter const& scene)
{
    // ********************************************* //
    // ********************************************* //
    //
    // TO DO: Calculer la couleur associee a un rayon dans la scene 3D
    //
    // ********************************************* //
    // ********************************************* //
    //Le code suivant affecte la couleur de base de la premiere intersection trouvee
    //Ce code ne gere pas les reflections.


    intersection_data intersection; //current intersection
    intersection_data inter;
    int intersected_primitive = 0;  //current index of intersected primitive
    int N = 5;
    bool is_int;
    color c = {0,0,0};
    vec3 up;
    vec3 u;
    ray ri;
    material mat2;
    float coef = 1.0f;
    bool const is_intersection = compute_intersection(r,scene,intersection,intersected_primitive);
    if(is_intersection) //if the ray intersects a primitive
    {
       // return intersection.relative/5.;
        material const& mat = scene.get_material(intersected_primitive);
        ri = r;
        c = compute_illumination(mat,intersection,scene);
        up = -1 * reflected(ri.u(), intersection.normal);
        for(int k = 0; k < N ; ++k){
           // up = reflected(ri.u(), intersection.normal);
            ri  = ray(intersection.position, up);
            ri.offset();
            is_int = compute_intersection(ri, scene, intersection, intersected_primitive);
            if(is_int){
                mat2 = scene.get_material(intersected_primitive);
                up = -reflected(up, intersection.normal);
                if(mat2.reflection() < 1)
                    c += compute_illumination(mat2,intersection,scene) * pow(mat2.reflection(),2*k+1);
            }else
                break;
        }
        return c;

    }
    else
        return color(0,0,0); //no intersection
}


bool compute_intersection(ray const& r,
                          scene_parameter const& scene,
                          intersection_data& intersection,
                          int& index_intersected_primitive)
{
    // ********************************************* //
    // ********************************************* //
    //
    // TO DO: Calculer l'intersection valide la plus proche
    //        Doit retourner vrai/faux si une intersection est trouvee ou non
    //        Doit mettre a jour intersection et index_intersected_primitive avec l'intersection la plus proche
    //
    // ********************************************* //
    // ********************************************* //

    //Le code suivant renvoie la premiere intersection valide que l'on trouve dans l'ordre de stockage du vecteur
    //Ce code est a modifier afin de trouver la 1ere intersection dans l'espace 3D le long du rayon.

    int const N_primitive = scene.size_primitive();
    //vec3 const&  p0 = ray.p0();
    bool found_intersection = false;
    //float t = intersection.relative;
    intersection_data inter2;


    float dist = std::numeric_limits<float>::max();
    int k;
    for(k = 0; k < N_primitive; ++k){
        primitive_basic const & primitive = scene.get_primitive(k);
        bool is_intersection = primitive.intersect(r,inter2);
        if(is_intersection){

            if(inter2.relative < dist)
            {
                dist = inter2.relative;
                index_intersected_primitive = k ;
                intersection = inter2;
            }
            found_intersection = true;
       }
    }
    return found_intersection;
}


bool is_in_shadow(vec3 const& p,vec3 const& p_light, scene_parameter const& scene)
{
    // ********************************************* //
    //
    // TO DO: Calculer si le point p est dans l'ombre de la lumiere situee en p_light
    //
    // ********************************************* //


    int const N_primitive = scene.size_primitive();

    bool found_intersection = false;
    ray r(p, p_light - p);
    r.offset(pow(10,-4));
    //float t = intersection.relative;
    float dist = std::numeric_limits<float>::max();
    int k;
    intersection_data intersection;
    for(k = 0; k < N_primitive; ++k){
        primitive_basic const & primitive = scene.get_primitive(k);
        bool is_intersection = primitive.intersect(r,intersection);
        if(is_intersection){
        return true;
       }
    }

    return false;

}



color compute_illumination(material const& mat,intersection_data const& intersection,scene_parameter const& scene)
{
    // ********************************************* //
    //
    // TO DO: Mettre en place le calcul d'illumination correct
    //
    //   Pour toutes les lumieres
    //     Si point dans l'ombre
    //       Ajouter a couleur courante une faible part de la couleur du materiau
    //          ex. c += mat.color_object()/10.0f;
    //     Sinon
    //       Calculer illumination au point courant en fonction de la position
    //          de la lumiere et de la camera
    //       Ajouter a couleur courante la contribution suivante:
    //           puissance de la lumiere (L.power)
    //         X couleur de la lumiere (L.c)
    //         X valeur de l'illumination
    //
    // ********************************************* //

    color c;
    shading_parameter s;
    vec3 const& p0 = intersection.position;

    int const N_light = scene.size_light();
    for(int k=0; k<N_light ; ++k)
    {
        light const& L = scene.get_light(k);
        bool const shadow = is_in_shadow(p0,L.p,scene);
        if(shadow == true)
            c += mat.color_object()/10.0f;
        else{
            //std::cout<<L.power;
            c +=  compute_shading(s, mat.color_object(), p0, L.p, intersection.normal, scene.get_camera().center());
        }
    }

    return c;


}





}


