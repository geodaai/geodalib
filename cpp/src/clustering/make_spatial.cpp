// SPDX-License-Identifier: MIT
// Copyright contributors to the geodalib project

#include <algorithm>
#include <set>
#include <stack>
#include <vector>

#include "make_spatial.h"
#include "../weights/geoda-weight.h"

namespace geoda {

MakeSpatialComponent::MakeSpatialComponent(int cid, const std::vector<int>& elements, GeoDaWeight* weights,
                                           std::map<int, int>& cluster_dict)
    : cid(cid), elements(elements), weights(weights), cluster_dict(cluster_dict) {
  int num_elements = static_cast<int>(elements.size());
  isSingleton = num_elements == 1;
  isIsland = isSingleton && weights->GetNeighbors(elements[0]).empty();

  isSurroundedSingleton = false;
  if (isSingleton) {
    std::vector<unsigned int> nbrs = weights->GetNeighbors(elements[0]);
    std::map<int, bool> nbr_dict;
    for (size_t i = 0; i < nbrs.size(); ++i) {
      if (elements[0] != static_cast<int>(nbrs[i])) {
        // operator[] would insert a default 0 for a neighbor id missing from
        // cluster_dict; look up without mutating and skip unknown neighbors.
        std::map<int, int>::const_iterator it =
            cluster_dict.find(static_cast<int>(nbrs[i]));
        if (it != cluster_dict.end()) {
          nbr_dict[it->second] = true;
        }
      }
    }
    isSurroundedSingleton = nbr_dict.size() == 1;
  }

  for (int i = 0; i < num_elements; ++i) {
    elements_dict[elements[i]] = true;
  }
}

MakeSpatialComponent::~MakeSpatialComponent() {}

bool MakeSpatialComponent::Has(int eid) {
  std::map<int, bool>::const_iterator it = elements_dict.find(eid);
  return it != elements_dict.end() && it->second;
}

void MakeSpatialComponent::Merge(MakeSpatialComponent* comp) {
  const std::vector<int>& new_elements = comp->GetElements();
  for (size_t i = 0; i < new_elements.size(); ++i) {
    this->elements.push_back(new_elements[i]);
    this->elements_dict[new_elements[i]] = true;
  }
}

MakeSpatialCluster::MakeSpatialCluster(int cid, const std::vector<int>& elements, GeoDaWeight* weights,
                                       std::map<int, int>& cluster_dict)
    : cid(cid), elements(elements), cluster_dict(cluster_dict), weights(weights), core(0) {
  int num_elements = static_cast<int>(elements.size());

  std::map<int, bool> visited;
  for (int i = 0; i < num_elements; ++i) {
    int eid = elements[i];
    if (visited[eid]) {
      continue;
    }
    std::vector<int> component;
    visited[eid] = true;

    std::stack<int> stack;
    stack.push(eid);

    while (!stack.empty()) {
      int tmp_id = stack.top();
      stack.pop();
      component.push_back(tmp_id);
      std::vector<unsigned int> nbrs = weights->GetNeighbors(tmp_id);
      for (size_t j = 0; j < nbrs.size(); ++j) {
        int neighbor = static_cast<int>(nbrs[j]);
        // A neighbor missing from cluster_dict cannot belong to this cluster;
        // use find so the lookup never inserts a default mapping.
        std::map<int, int>::const_iterator it = cluster_dict.find(neighbor);
        if (it != cluster_dict.end() && it->second == this->cid && !visited[neighbor]) {
          visited[neighbor] = true;
          stack.push(neighbor);
        }
      }
    }

    MakeSpatialComponent* c = new MakeSpatialComponent(this->cid, component, weights, cluster_dict);
    components.push_back(c);

    for (size_t j = 0; j < component.size(); ++j) {
      int eid_c = component[j];
      component_dict[eid_c] = c;
    }

    if (core == 0 || core->GetSize() < c->GetSize()) {
      core = c;
    }
  }
}

MakeSpatialCluster::~MakeSpatialCluster() {
  for (size_t i = 0; i < components.size(); ++i) {
    delete components[i];
  }
}

MakeSpatialComponent* MakeSpatialCluster::GetComponent(int eid) {
  // find instead of operator[] so a read-only query never inserts a null entry
  // into component_dict for an unknown element.
  std::map<int, MakeSpatialComponent*>::const_iterator it = component_dict.find(eid);
  return it == component_dict.end() ? nullptr : it->second;
}

const std::vector<int>& MakeSpatialCluster::GetCoreElements() {
  // core stays null when the cluster is empty (an invalid partition that the
  // MakeSpatial constructor rejects); stay null-safe so no accessor dereferences
  // a null pointer.
  static const std::vector<int> empty;
  return core ? core->GetElements() : empty;
}

bool MakeSpatialCluster::BelongsToCore(int eid) { return core != nullptr && core->Has(eid); }

int MakeSpatialCluster::GetCoreSize() { return core ? core->GetSize() : 0; }

std::vector<MakeSpatialComponent*> MakeSpatialCluster::GetSurroundedSingletons() {
  std::vector<MakeSpatialComponent*> result;
  for (size_t i = 0; i < components.size(); ++i) {
    MakeSpatialComponent* comp = components[i];
    if (comp != core && comp->isSurroundedSingleton) {
      result.push_back(comp);
    }
  }
  return result;
}

std::vector<int> MakeSpatialCluster::GetComponentSize() {
  std::set<int> sz_set;
  for (size_t i = 0; i < components.size(); ++i) {
    MakeSpatialComponent* comp = components[i];
    if (comp != this->core) {
      sz_set.insert(comp->GetSize());
    }
  }
  return std::vector<int>(sz_set.begin(), sz_set.end());
}

int MakeSpatialCluster::GetSmallestComponentSize() {
  int result = -1;
  for (size_t i = 0; i < this->components.size(); ++i) {
    if (components[i] != core) {
      int sz = components[i]->GetSize();
      if (result < 0 || sz < result) {
        result = sz;
      }
    }
  }
  return result;
}

std::vector<MakeSpatialComponent*> MakeSpatialCluster::GetComponentsBySize(int component_size) {
  std::vector<MakeSpatialComponent*> result;
  for (size_t i = 0; i < this->components.size(); ++i) {
    MakeSpatialComponent* comp = this->components[i];
    if (comp != core && comp->GetSize() == component_size) {
      result.push_back(comp);
    }
  }
  return result;
}

void MakeSpatialCluster::MergeComponent(MakeSpatialComponent* from, MakeSpatialComponent* to) {
  for (size_t i = 0; i < components.size(); ++i) {
    if (components[i] == to) {
      to->Merge(from);
      const std::vector<int>& new_elements = from->GetElements();
      for (size_t j = 0; j < new_elements.size(); ++j) {
        int eid = new_elements[j];
        component_dict[eid] = to;
      }
      if (to->GetSize() > core->GetSize()) {
        core = to;
      }
      break;
    }
  }
}

void MakeSpatialCluster::RemoveComponent(MakeSpatialComponent* comp) {
  // Erase the removed elements unconditionally: the component's cluster id is
  // never updated before removal, so keeping the erase behind a cid check left
  // dangling pointers in component_dict after the component was deleted.
  const std::vector<int>& removed_elements = comp->GetElements();
  for (size_t i = 0; i < removed_elements.size(); ++i) {
    int eid = removed_elements[i];
    component_dict.erase(eid);
  }

  for (size_t i = 0; i < components.size(); ++i) {
    if (components[i] != core && components[i] == comp) {
      delete comp;
      components.erase(components.begin() + static_cast<long>(i));
      break;
    }
  }
}

MakeSpatial::MakeSpatial(int num_obs, const std::vector<std::vector<int>>& clusters, GeoDaWeight* weights)
    : num_obs(num_obs), clusters(clusters), weights(weights), valid(true) {
  num_clusters = static_cast<int>(clusters.size());

  for (int i = 0; i < num_clusters; ++i) {
    std::vector<int> cluster = clusters[i];
    // An empty cluster would leave MakeSpatialCluster with a null core that a
    // later accessor would dereference; reject the whole partition up front.
    // (Out-of-range cluster elements also fail the cluster_dict.size() !=
    // num_obs check below.)
    if (cluster.empty()) {
      valid = false;
    }
    for (auto j : cluster) {
      cluster_dict[j] = i;
    }
  }

  if (static_cast<int>(cluster_dict.size()) != num_obs) {
    valid = false;
  }

  for (int i = 0; i < num_clusters; ++i) {
    sk_clusters.push_back(new MakeSpatialCluster(i, clusters[i], weights, cluster_dict));
  }
}

MakeSpatial::~MakeSpatial() {
  for (size_t i = 0; i < sk_clusters.size(); ++i) {
    delete sk_clusters[i];
  }
}

struct ClusterSmall {
  bool operator()(MakeSpatialCluster* left, MakeSpatialCluster* right) const {
    return left->GetCoreSize() > right->GetCoreSize();
  }
};

int MakeSpatial::GetSmallestComponentSize() {
  int result = -1;
  for (int i = 0; i < num_clusters; ++i) {
    int sz = sk_clusters[i]->GetSmallestComponentSize();
    if (result < 0 || (sz > 0 && sz < result)) {
      result = sz;
    }
  }
  return result;
}

std::vector<MakeSpatialCluster*> MakeSpatial::GetClustersByComponentSize(int sz) {
  std::vector<MakeSpatialCluster*> result;
  for (int i = 0; i < num_clusters; ++i) {
    if (sk_clusters[i]->GetSmallestComponentSize() == sz) {
      result.push_back(sk_clusters[i]);
    }
  }
  return result;
}

void MakeSpatial::Run() {
  if (!valid) return;

  for (int i = 0; i < num_clusters; ++i) {
    std::vector<MakeSpatialComponent*> moved_comps = sk_clusters[i]->GetSurroundedSingletons();
    for (size_t j = 0; j < moved_comps.size(); ++j) {
      MakeSpatialComponent* moved_comp = moved_comps[j];
      this->MoveComponent(moved_comp);
    }
  }

  int N;
  while ((N = GetSmallestComponentSize()) > 0) {
    std::vector<MakeSpatialCluster*> cands = GetClustersByComponentSize(N);

    while (!cands.empty()) {
      std::make_heap(cands.begin(), cands.end(), ClusterSmall());
      std::pop_heap(cands.begin(), cands.end());

      MakeSpatialCluster* c = cands.back();
      cands.pop_back();

      std::vector<MakeSpatialComponent*> moved_comps = c->GetComponentsBySize(N);
      for (size_t j = 0; j < moved_comps.size(); ++j) {
        MakeSpatialComponent* moved_comp = moved_comps[j];
        this->MoveComponent(moved_comp);
      }
    }
  }
}

void MakeSpatial::MoveComponent(MakeSpatialComponent* comp) {
  int largest_size = 0;
  MakeSpatialComponent* best_to = 0;

  const std::vector<int>& elements = comp->GetElements();
  for (size_t i = 0; i < elements.size(); ++i) {
    int eid = elements[i];
    std::vector<unsigned int> nbrs = weights->GetNeighbors(eid);
    for (size_t j = 0; j < nbrs.size(); ++j) {
      int nbr = static_cast<int>(nbrs[j]);
      if (!comp->Has(nbr)) {
        // A neighbor id missing from cluster_dict has no target cluster; skip
        // it rather than letting operator[] insert a default mapping.
        std::map<int, int>::const_iterator it = this->cluster_dict.find(nbr);
        if (it == this->cluster_dict.end()) {
          continue;
        }
        int target = it->second;
        MakeSpatialComponent* to = sk_clusters[target]->GetComponent(nbr);
        if (to != 0 && to != comp && to->GetSize() > largest_size) {
          best_to = to;
          largest_size = to->GetSize();
        }
      }
    }
  }
  if (best_to != 0) {
    UpdateComponent(comp, best_to);
  } else {
    valid = false;
  }
}

void MakeSpatial::UpdateComponent(MakeSpatialComponent* moved_comp, MakeSpatialComponent* target) {
  const std::vector<int>& elements = moved_comp->GetElements();
  for (size_t i = 0; i < elements.size(); ++i) {
    int eid = elements[i];
    cluster_dict[eid] = target->GetClusterId();
  }

  int to = target->GetClusterId();
  int from = moved_comp->GetClusterId();

  sk_clusters[to]->MergeComponent(moved_comp, target);
  sk_clusters[from]->RemoveComponent(moved_comp);
}

std::vector<std::vector<int>> MakeSpatial::GetClusters() {
  int total_core_obs = 0;
  for (int i = 0; i < num_clusters; ++i) {
    MakeSpatialCluster* skc = sk_clusters[i];
    total_core_obs += skc->GetCoreSize();
  }

  if (total_core_obs != num_obs) {
    valid = false;
    return this->clusters;
  }

  std::vector<std::vector<int>> result;
  for (int i = 0; i < num_clusters; ++i) {
    MakeSpatialCluster* skc = sk_clusters[i];
    result.push_back(skc->GetCoreElements());
  }
  return result;
}

}  // namespace geoda
