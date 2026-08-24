// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#ifndef GEODA_CLUSTERING_MAKE_SPATIAL_H
#define GEODA_CLUSTERING_MAKE_SPATIAL_H

#include <map>
#include <vector>

class GeoDaWeight;

namespace geoda {

class MakeSpatialCluster;
class MakeSpatial;

class MakeSpatialComponent {
 public:
  MakeSpatialComponent(int cid, const std::vector<int>& elements, GeoDaWeight* weights,
                       std::map<int, int>& cluster_dict);
  virtual ~MakeSpatialComponent();

  int GetClusterId() { return cid; }
  void SetClusterId(int cid) { this->cid = cid; }

  int GetSize() { return static_cast<int>(elements.size()); }
  // Return by const reference: callers iterate this membership without
  // mutating it, so copying on every call (merges, move/update paths, core
  // lookups) would repeatedly allocate large vectors for big datasets.
  const std::vector<int>& GetElements() { return elements; }

  void Merge(MakeSpatialComponent* comp);

  bool Has(int eid);

  bool isIsland;
  bool isSingleton;
  bool isSurroundedSingleton;

 protected:
  int cid;
  std::vector<int> elements;
  GeoDaWeight* weights;
  std::map<int, int>& cluster_dict;
  std::map<int, bool> elements_dict;
};

class MakeSpatialCluster {
 public:
  MakeSpatialCluster(int cid, const std::vector<int>& elements, GeoDaWeight* weights,
                     std::map<int, int>& cluster_dict);
  virtual ~MakeSpatialCluster();

  std::vector<MakeSpatialComponent*> GetSurroundedSingletons();
  std::vector<MakeSpatialComponent*> GetComponentsBySize(int component_size);
  void MergeComponent(MakeSpatialComponent* from, MakeSpatialComponent* to);
  void RemoveComponent(MakeSpatialComponent* comp);
  const std::vector<int>& GetCoreElements();
  int GetCoreSize();
  int GetComponentSize(int eid);
  int GetSmallestComponentSize();
  MakeSpatialComponent* GetComponent(int eid);
  bool BelongsToCore(int eid);
  std::vector<int> GetComponentSize();

 protected:
  int cid;
  std::vector<int> elements;
  std::map<int, int>& cluster_dict;
  GeoDaWeight* weights;
  MakeSpatialComponent* core;
  std::vector<MakeSpatialComponent*> components;
  std::map<int, MakeSpatialComponent*> component_dict;
};

class MakeSpatial {
 public:
  MakeSpatial(int num_obs, const std::vector<std::vector<int>>& clusters, GeoDaWeight* weights);
  virtual ~MakeSpatial();

  void Run();
  std::vector<std::vector<int>> GetClusters();
  bool IsValid() { return valid; }

 protected:
  void UpdateComponent(MakeSpatialComponent* moved_comp, MakeSpatialComponent* target);
  void MoveComponent(MakeSpatialComponent* comp);
  int GetSmallestComponentSize();
  std::vector<MakeSpatialCluster*> GetClustersByComponentSize(int sz);

 protected:
  int num_obs;
  std::vector<std::vector<int>> clusters;
  GeoDaWeight* weights;
  bool valid;
  int num_clusters;
  std::map<int, int> cluster_dict;
  std::vector<MakeSpatialCluster*> sk_clusters;
};

}  // namespace geoda

#endif
