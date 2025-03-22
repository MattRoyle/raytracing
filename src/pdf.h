#ifndef PDF_H
#define PDF_H

#include "hittable_list.h"
#include "onb.h"


class pdf {
  public:
    virtual ~pdf() {}

    //return pdf distribution value of the pdf distribution in a given direction
    virtual double value(const vec3& direction) const = 0;
    //return the random direction weighted towards a pdf distribution
    virtual vec3 generate() const = 0;
};

class uniform_sphere_pdf : public pdf {
    public:
        uniform_sphere_pdf(){};
    
    virtual double value(const vec3& direction) const override {
        return 1 / (4*PI);
    }
    virtual vec3 generate() const override{
        return random_unit_vector();
    }
};

class cosine_pdf : public pdf {
    public:
        cosine_pdf(const vec3& w) : uvw(w) {}
  
        double value(const vec3& direction) const override {
            auto cos_theta = dot(unit_vector(direction), uvw.w());
            return cos_theta < 0 ? 0 : cos_theta / PI;
        }
        
  
        vec3 generate() const override {
            return uvw.transform(random_cosine_direction());
        }
  
    private:
        onb uvw;
};

class hittable_pdf : public pdf {
    public:
        hittable_pdf(const hittable& objects, const point3& origin) 
            : objects(objects), origin(origin) {

            }
  
        double value(const vec3& direction) const override {
            return objects.pdf_value(origin, direction);
        }
        
        vec3 generate() const override {
            return objects.random(origin);
        }
  
    private:
        const hittable& objects;
        point3 origin;
};

class mixture_pdf : public pdf {
    public:
        mixture_pdf(shared_ptr<pdf> pdf0, shared_ptr<pdf> pdf1) {
          pdfs[0] = pdf0;
          pdfs[1] = pdf1;
      }
  
      double value(const vec3& direction) const override {
          return 0.5 * pdfs[0]->value(direction) + 0.5 *pdfs[1]->value(direction);
      }
      
      vec3 generate() const override {
          if (random_double() < 0.5)//randomly choose which to use
              return pdfs[0]->generate();
          else
              return pdfs[1]->generate();
      }
  
    private:
      shared_ptr<pdf> pdfs[2];
  };
  
#endif