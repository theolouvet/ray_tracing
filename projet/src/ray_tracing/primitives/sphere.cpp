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

#include "sphere.hpp"

#include "intersection_data.hpp"
#include "../scene/ray.hpp"
#include "lib/common/error_handling.hpp"

#include <cmath>

namespace cpe
{

sphere::sphere(vec3 const& center_param,float radius_param)
    :center_data(center_param),radius_data(radius_param)
{}

vec3 const& sphere::center() const
{
    return center_data;
}
float sphere::radius() const
{
    return radius_data;
}

bool sphere::intersect(ray const& ray_param,intersection_data& intersection) const
{

    vec3 const& u = ray_param.u();

    // ********************************************************** //
    // ********************************************************** //
    //  TO DO:
    //    Calcul d'intersection entre un rayon et une plan
    //
    // Variales:
    //  - Position initiale du rayon: ray_param.p0()
    //  - Vecteur directeur unitaire du rayon: u
    //  - Position du centre de la sphere: center_data
    //  - Rayon de la sphere: radius_data
    //
    // Aide de syntaxe:
    //  - Norme au carre d'un vecteur v: float norme=v.norm2();
    //             ou: float norme=v.dot(v);
    //
    // ********************************************************** //
    // ********************************************************** //



    //Le code suivant est arbitraire est doit etre modifie
    vec3 const& p0 = ray_param.p0();
    vec3 const& p = center_data;
    vec3 const& pp0 = p0 - p;
    //float f=sqrt(4.0f*dot(p0-center_data,u)*dot(p0-1.1f*center_data,u)-3.5f*dot(p0-center_data,p0-center_data));
    //float b = 2.0f * ( p0.x()*u.x() + p0.y()*u.y() + p0.z()*u.z() );
    float b= 2.0f * dot(pp0,u);
    float a = 1.0f;
    float eps = 0.005f;
    float c = pow(pp0.x(),2)+pow(pp0.y(),2)+pow(pp0.z(),2)-pow(radius_data,2);
    float delta = 1.0f * pow(b,2) - 4.0f * a *c;
    float f = 0.0f;
    if(delta == 0 )
        f = -1.0 * b / (2.0f * a);
    else if(delta > 0){
        f = (-1.0f * b - (float) sqrt(delta))/(2.0f * a);
        if(f < 0){
            f = (-1.0f * b + (float) sqrt(delta))/(2.0f * a);
        }
    }else
        return false;

    vec3 const& pinter = p0+f*u;
    vec3 const& ninter = normalized(pinter - p);
    intersection.set(pinter, ninter,f);
    //intersection.set();
    return true;


}



}
