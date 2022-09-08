#include "logging/logging.h"
#include "component/component.h"
#include "collisions.h"
#include <tuple>
#include <float.h>
using namespace physics;


// TODO: optimise
// TODO: spatial data structure analysis to optimise collisions

inline float getSign (glm::vec2 vec, glm::vec2 basis) {
    if (vec.x * basis.x >= 0 && vec.y * basis.y >= 0) {
        return 1.0f;
    } else {
        return -1.0f;
    }
}

inline std::pair<bool,glm::vec2> entityCollision (CollisionComponent& comp1, CollisionComponent& comp2, glm::vec2 displacement) {
    if (!(comp1.layers & comp2.masks) && !(comp2.layers & comp1.masks)) {
        return std::pair(false, glm::vec2({0,0}));
    }
    //glm::vec2 displacement = comp1.pos - comp2.pos;
    float outerRadiusDist = comp1.outerRadius + comp2.outerRadius;
    if (squaredDistance(displacement) >
            outerRadiusDist * outerRadiusDist) {
        return std::pair(false, glm::vec2({0,0}));
    }
    glm::mat2 inv1 = glm::inverse(comp1.rotBBMap);
    glm::mat2 inv2 = glm::inverse(comp2.rotBBMap);

    glm::vec2 basisVec[2] = {{1,0}, {0,1}};
    float leastSquaredDist = FLT_MAX;
    glm::vec2 smallestDispVec;

    for (int i = 0; i < 4; i++) {
        glm::vec2 basis1 = basisVec[i % 2];
        bool isIntersection = false;
        glm::vec2 minIntersectVec = {1.1f, 1.1f};
        glm::vec2 maxIntersectVec = {0.0f, 0.0f};
        float posMax = 0.0f;
        float negMax = -0.0f;
        glm::mat2 transMat = i < 2 ? comp1.rotBBMap : comp2.rotBBMap;
        glm::mat2 invMat = i < 2 ? inv2 : inv1;
        glm::vec2 relDisplacement = i < 2 ? displacement : -displacement;
        for (unsigned int j = 0; j < 4; j++) {
            glm::vec2 testPoint = {0, 0};
            if (j % 2 == 0) {
                testPoint -= basisVec[0];
            } else {
                testPoint += basisVec[0];
            }
            if (j / 2 == 0) {
                testPoint -= basisVec[1];
            } else {
                testPoint += basisVec[1];
            }

            glm::vec2 transPoint = transMat * testPoint + relDisplacement;

            glm::vec2 relPoint = projectVec(basis1, invMat * transPoint);

            if (squaredDistance(relPoint) < squaredDistance(minIntersectVec)) {
                minIntersectVec = relPoint;
            } else if (squaredDistance(relPoint) > squaredDistance(maxIntersectVec)){
                maxIntersectVec = relPoint;
            }

            if (squaredDistance(relPoint) <= 1) {
                /*// intersection
                if (1 - squaredDistance(relPoint) < leastSquaredDist) {
                    glm::vec2 overlapVec;
                    if (relPoint.x >= 0 && relPoint.y >= 0) {
                        overlapVec = basis1 - relPoint;
                    } else {
                        overlapVec = - basis1 - relPoint;
                    }
                    leastSquaredDist = 1 - squaredDistance(relPoint);
                    smallestDispVec = (i < 2 ? comp2.bbMap : comp1.bbMap)  *  overlapVec * (i < 2 ? 1.0f : -1.0f);
                }*/

                isIntersection = true;
                //break;
            }
            float sign = getSign(relPoint, basis1);
            float mul = glm::dot(basis1, relPoint) / squaredDistance(basis1);
            if (mul >= posMax) {
                posMax = mul;
            } else if (mul <= negMax) {
                negMax = mul;
            }
        }

        if (isIntersection) {
            float sign = getSign(maxIntersectVec - minIntersectVec, basis1);
            glm::vec2 overlapVec = basis1 * sign - minIntersectVec;
            auto dispVec = (i < 2 ? comp2.rotBBMap : comp1.rotBBMap) * overlapVec;
            if (squaredDistance(dispVec) < leastSquaredDist) {
                smallestDispVec = dispVec * (i < 2 ? 1.0f : -1.0f);
                leastSquaredDist = squaredDistance(dispVec);
            }
        } else if (posMax >= 1.0f && negMax <= -1.0f) {
            float minDist;
            if (posMax + 1.0f >= -(negMax - 1.0f)) {
                minDist = posMax + 1.0f;
            } else {
                minDist = negMax - 1.0f;
            }
            glm::vec2 overlapVec = basis1 * minDist;
            auto dispVec = (i < 2 ? comp2.rotBBMap : comp1.rotBBMap) * overlapVec;
            if (squaredDistance(dispVec) < leastSquaredDist) {
                smallestDispVec = dispVec * (i < 2 ? 1.0f : -1.0f);;
                leastSquaredDist = squaredDistance(dispVec);
            }
        } else {
            return std::pair(false, glm::vec2({0,0}));
        }
    }
    //logging::log(LEVEL_DEBUG, "Smallest displacement: <{}, {}> (epsilon = {})", smallestDispVec.x, smallestDispVec.y, FLT_EPSILON);
    if (fabs(smallestDispVec.x) < FLT_EPSILON && fabs(smallestDispVec.y) < FLT_EPSILON) {
        return std::pair(false, glm::vec2{0,0});
    }
    return std::pair(true, smallestDispVec);
}

/*void physics::checkCollisionsEntity (CollisionComponent* comp, component::EntityId* ids, int numEntities, [[maybe_unused]] int direction, std::vector<std::tuple<component::EntityId, component::EntityId, glm::vec2>>* collisionResults) {
    for (int i = 0; i < numEntities; i++) {
        auto compPtr = comp + 1;
        for (int j = i + 1; j < numEntities; j++) {
            auto coll = entityCollision(*comp, *compPtr);
            if (coll.first) {
                collisionResults->emplace_back(std::tuple(ids[i], ids[j], coll.second));
            }
            compPtr++;
        }
        comp++;
    }
}*/

void physics::checkCollisionsEntity (const component::EntityComponentManager::SharedPtr& compManager, std::vector<std::tuple<component::EntityId, component::EntityId, glm::vec2>>& collisionResults) {
    /*for (auto i = compManager->begin(); i != compManager->end(); i++) {
        for (auto j = i + 1; j != compManager->end(); j++) {
            (*i).applyFunc<component::EntityId, CollisionComponent>([&j, &collisionResults](component::EntityId& id1, CollisionComponent& comp1) {
                (*j).applyFunc<component::EntityId, CollisionComponent>([&](component::EntityId& id2, CollisionComponent& comp2) {
                    auto coll = entityCollision(comp1, comp2);
                    if (coll.first) {
                        collisionResults.emplace_back(id1, id2, coll.second);
                    }
                });
            });
        }
    }*/
    auto consView = compManager->getConstrainedView<CollisionComponent, component::EntityMainComponent>();

    for (auto i = consView.begin(); i != consView.end(); i++) {
        auto j = i;
        j++;
        /*if (j == consView.end()) {
            break;
        }*/
        for ( ; j != consView.end(); j++) {
            auto coll = entityCollision((*i).get<CollisionComponent>(), (*j).get<CollisionComponent>(),
                    (*i).get<component::EntityMainComponent>().pos - (*j).get<component::EntityMainComponent>().pos);
            if (coll.first) {
                collisionResults.emplace_back((*i).getId(), (*j).getId(), coll.second);
            }
        }
    }
}

util::DataValue physics::CollisionComponent::serialise () const {
    util::DataObject obj;
    util::DataArray collScale;
    for (int i = 0; i < 2; i++) {
        collScale.push_back(bbMap[i]);
    }
    obj["coll_scale"] = std::move(collScale);
    obj["mass"] = mass;
    obj["layers"] = layers;
    obj["masks"] = masks;
    obj["resolve_layers"] = resolveLayers;
    obj["event_layers"] = eventLayer;
    return (util::DataValue) obj;
}

void physics::CollisionComponent::deserialise (const util::DataValue& val) {
    auto& obj = val.get<util::DataObject>();
    if (obj.contains("coll_scale")) {
        // TODO
        auto& arr = obj.at("coll_scale").get<util::DataArray>();
        bbMap[0] = arr[0].get<glm::vec2>();
        bbMap[1] = arr[1].get<glm::vec2>();
    }
    if (obj.contains("mass")) {
        mass = obj.at("mass").get<float>();
    }
    if (obj.contains("layers")) {
        layers = obj.at("layers").get<int>();
    }
    if (obj.contains("masks")) {
        masks = obj.at("masks").get<int>();
    }
    if (obj.contains("resolve_layers")) {
        resolveLayers = obj.at("resolve_layers").get<int>();
    }
    if (obj.contains("event_layers")) {
        eventLayer = obj.at("event_layers").get<int>();
    }
    outerRadius = glm::sqrt(glm::dot(bbMap[0], bbMap[0]) + glm::dot(bbMap[1],bbMap[1]));
    rotBBMap = bbMap;
}