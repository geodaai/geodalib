// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#ifndef __GEODA_CENTER_SPATIAL_VALIDATION_H__
#define __GEODA_CENTER_SPATIAL_VALIDATION_H__

#include <vector>
#include <map>

#include "geofeature.h"
#include "gda_clustering.h"

class GeoDaWeight;
class SpatialValidationCluster;
class SpatialValidation;


class SpatialValidationComponent
{
public:
    SpatialValidationComponent(int cid, const std::vector<int>& elements,
                               GeoDaWeight* weights, std::map<int, int>& cluster_dict,
                               std::map<int, std::vector<int> >& edges,
                               int nCPU=4);
    virtual ~SpatialValidationComponent();
    
    int GetClusterId() { return cid; }
    void SetClusterId(int cid) { this->cid = cid; }
    
    int GetSize() { return (int)elements.size(); }
    std::vector<int> GetElements() { return elements; }
        
    bool Has(int eid);
    
    Diameter ComputeDiameter();

    void ComputeDiameterThread(int start, int end);

    bool isIsland;
    bool isSingleton;
    bool isSurroundedSingleton;
    
protected:
    int cid;
    std::vector<int> elements;
    GeoDaWeight* weights;
    std::map<int, int>& cluster_dict;
    std::map<int, std::vector<int> > edges;
    int nCPUs;
    std::map<int, bool> elements_dict;
    
    struct Step {
        int eid;
        std::map<int, int>& steps;
        Step(int eid, std::map<int, int>& steps) : eid(eid), steps(steps){}
        bool operator < (const Step& item) const {
            return steps[eid] > steps[item.eid];
        }
        Step& operator = (const Step& item) {
            eid = item.eid;
            steps = item.steps;
            return *this;
        }
    };
    
    std::vector<int> shortest_paths;
};

class SpatialValidationCluster
{
public:
    SpatialValidationCluster(int cid, const std::vector<int>& elements, GeoDaWeight* weights,
                             std::map<int, int>& cluster_dict,
                             std::vector<gda::GeometryContent*>& geoms,
                             gda::ShapeType shape_type);
    virtual ~SpatialValidationCluster();
    
    std::vector<int> GetCoreElements();
    
    int GetSize();
    
    int GetCoreSize();
    
    int GetComponentSize();
    
    SpatialValidationComponent* GetComponent(int eid);
        
    Fragmentation ComputeFragmentation();
    
    Compactness ComputeCompactness();
    
    Diameter ComputeDiameter();
    
protected:
    int cid;
    std::vector<int> elements;
    std::map<int, int>& cluster_dict;
    GeoDaWeight* weights;
    SpatialValidationComponent* core;
    std::vector<gda::GeometryContent*> geoms;
    gda::ShapeType shape_type;
    std::vector<SpatialValidationComponent*> components;
    std::map<int, SpatialValidationComponent*> component_dict;
    
};



class SpatialValidation
{
public:
    SpatialValidation(int num_obs, const std::vector<std::vector<int> >& clusters,
                      GeoDaWeight* weights,
                      std::vector<gda::GeometryContent*>& geoms,
                      gda::ShapeType shape_type);
	virtual ~SpatialValidation();
            
    bool IsValid() { return valid; }
    
    Fragmentation GetFragmentation() { return fragmentation; }
        
    std::vector<Fragmentation> GetFragmentationFromClusters() { return fragmentations; }
    
    std::vector<Compactness> GetCompactnessFromClusters() { return compactnesses; }
    
    std::vector<Diameter> GetDiameterFromClusters() { return diameters; }
        
    bool IsSpatiallyConstrained();
    
protected:
    void ComputeFragmentation();
    
    void ComputeCompactness();
    
    void ComputeDiameter();
    
protected:
    int num_obs;
    std::vector<std::vector<int> > clusters;
    GeoDaWeight* weights;
    bool valid;
    std::vector<gda::GeometryContent*> geoms;
    gda::ShapeType shape_type;
    
    int num_clusters;
    std::map<int, int> cluster_dict;
    std::vector<SpatialValidationCluster*> sk_clusters;
    
    Fragmentation fragmentation;
    std::vector<Fragmentation> fragmentations;
    std::vector<Compactness> compactnesses;
    std::vector<Diameter> diameters;
};

#endif
