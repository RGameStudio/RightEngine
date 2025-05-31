#include <doctest/doctest.h>
#include <Engine/Service/Resource/MaterialResource.hpp>
#include <Engine/Serialization/FromJson.hpp>
#include <Engine/Serialization/ToJson.hpp>
#include <Engine/Registration.hpp>

using namespace engine;

TEST_CASE("Material serialization/deserialization tests")
{
    SUBCASE("MaterialDependency serialization")
    {
        MaterialDependency dep;
        dep.path = "/System/Materials/pbr.material";
        dep.index = 0;
        dep.hasDependency = true;

        auto json = ToJsonObject(dep);

        CHECK_EQ(json["path"], "/System/Materials/pbr.material");
        CHECK_EQ(json["index"], 0);
        CHECK_EQ(json["hasDependency"], true);

        auto deserializedOpt = FromJsonString<MaterialDependency>(json.dump());
        CHECK(deserializedOpt.has_value());

        auto deserialized = deserializedOpt.value();
        CHECK_EQ(deserialized.path.generic_string(), "/System/Materials/pbr.material");
        CHECK_EQ(deserialized.index, 0);
        CHECK_EQ(deserialized.hasDependency, true);
    }

    SUBCASE("MaterialAttachment serialization")
    {
        MaterialAttachment attachment;
        attachment.loadOperation = rhi::AttachmentLoadOperation::LOAD;
        attachment.storeOperation = rhi::AttachmentStoreOperation::STORE;
        
        attachment.dependency.path = "/System/Materials/pbr.material";
        attachment.dependency.index = 0;
        attachment.dependency.hasDependency = true;

        auto json = ToJsonObject(attachment);

        CHECK_EQ(json["loadOperation"], "LOAD");
        CHECK_EQ(json["storeOperation"], "STORE");
        CHECK(json["dependency"].is_object());
        CHECK_EQ(json["dependency"]["path"], "/System/Materials/pbr.material");
        CHECK_EQ(json["dependency"]["index"], 0);
        CHECK_EQ(json["dependency"]["hasDependency"], true);

        auto deserializedOpt = FromJsonString<MaterialAttachment>(json.dump());
        CHECK(deserializedOpt.has_value());

        auto deserialized = deserializedOpt.value();
        CHECK_EQ(deserialized.loadOperation, rhi::AttachmentLoadOperation::LOAD);
        CHECK_EQ(deserialized.storeOperation, rhi::AttachmentStoreOperation::STORE);
        CHECK_EQ(deserialized.dependency.path, "/System/Materials/pbr.material");
        CHECK_EQ(deserialized.dependency.index, 0);
        CHECK_EQ(deserialized.dependency.hasDependency, true);
    }

    SUBCASE("MaterialAttachment without dependency")
    {
        MaterialAttachment attachment;
        attachment.loadOperation = rhi::AttachmentLoadOperation::CLEAR;
        attachment.storeOperation = rhi::AttachmentStoreOperation::STORE;
        attachment.dependency.hasDependency = false; // нет dependency

        auto json = ToJsonObject(attachment);

        CHECK_EQ(json["loadOperation"], "CLEAR");
        CHECK_EQ(json["storeOperation"], "STORE");
        CHECK_EQ(json["dependency"]["hasDependency"], false);

        auto deserializedOpt = FromJsonString<MaterialAttachment>(json.dump());
        CHECK(deserializedOpt.has_value());

        auto deserialized = deserializedOpt.value();
        CHECK_EQ(deserialized.loadOperation, rhi::AttachmentLoadOperation::CLEAR);
        CHECK_EQ(deserialized.storeOperation, rhi::AttachmentStoreOperation::STORE);
        CHECK_EQ(deserialized.dependency.hasDependency, false);
    }

    SUBCASE("MaterialData complete serialization")
    {
        MaterialData materialData;
        materialData.name = "skybox";
        materialData.shader = "/System/Shaders/skybox.glsl";
        materialData.version = 0;
        materialData.offscreen = true;
        materialData.depthCompareOp = "LESS_OR_EQUAL";
        materialData.cullMode = "FRONT";
        materialData.compute = false;
        
        // Добавляем attachment
        MaterialAttachment attachment;
        attachment.loadOperation = rhi::AttachmentLoadOperation::LOAD;
        attachment.storeOperation = rhi::AttachmentStoreOperation::STORE;
        attachment.dependency.path = "/System/Materials/pbr.material";
        attachment.dependency.index = 0;
        attachment.dependency.hasDependency = true;
        
        materialData.attachments.push_back(attachment);

        // Добавляем depth attachment
        materialData.depthAttachment.loadOperation = rhi::AttachmentLoadOperation::LOAD;
        materialData.depthAttachment.storeOperation = rhi::AttachmentStoreOperation::STORE;
        materialData.depthAttachment.dependency.path = "/System/Materials/pbr.material";
        materialData.depthAttachment.dependency.index = 0;
        materialData.depthAttachment.dependency.hasDependency = true;
        materialData.hasDepthAttachment = true;

        auto json = ToJsonObject(materialData);

        CHECK_EQ(json["name"], "skybox");
        CHECK_EQ(json["shader"], "/System/Shaders/skybox.glsl");
        CHECK_EQ(json["version"], 0);
        CHECK_EQ(json["offscreen"], true);
        CHECK_EQ(json["depthCompareOp"], "LESS_OR_EQUAL");
        CHECK_EQ(json["cullMode"], "FRONT");
        CHECK_EQ(json["compute"], false);
        CHECK(json["attachments"].is_array());
        CHECK_EQ(json["attachments"].size(), 1);
        CHECK(json["depthAttachment"].is_object());
        CHECK_EQ(json["hasDepthAttachment"], true);

        auto deserializedOpt = FromJsonString<MaterialData>(json.dump());
        CHECK(deserializedOpt.has_value());

        auto deserialized = deserializedOpt.value();
        CHECK_EQ(deserialized.name, "skybox");
        CHECK_EQ(deserialized.shader.generic_string(), "/System/Shaders/skybox.glsl");
        CHECK_EQ(deserialized.version, 0);
        CHECK_EQ(deserialized.offscreen, true);
        CHECK_EQ(deserialized.depthCompareOp, "LESS_OR_EQUAL");
        CHECK_EQ(deserialized.cullMode, "FRONT");
        CHECK_EQ(deserialized.compute, false);
        CHECK_EQ(deserialized.attachments.size(), 1);
        CHECK_EQ(deserialized.hasDepthAttachment, true);
        
        // Проверяем енумы в attachment'ах
        CHECK_EQ(deserialized.attachments[0].loadOperation, rhi::AttachmentLoadOperation::LOAD);
        CHECK_EQ(deserialized.attachments[0].storeOperation, rhi::AttachmentStoreOperation::STORE);
        CHECK_EQ(deserialized.depthAttachment.loadOperation, rhi::AttachmentLoadOperation::LOAD);
        CHECK_EQ(deserialized.depthAttachment.storeOperation, rhi::AttachmentStoreOperation::STORE);
    }

    SUBCASE("MaterialData with compute shader")
    {
        MaterialData materialData;
        materialData.name = "compute_test";
        materialData.shader = "/System/Shaders/compute.comp";
        materialData.version = 0;
        materialData.compute = true; // compute shader

        auto json = ToJsonObject(materialData);

        CHECK_EQ(json["name"], "compute_test");
        CHECK_EQ(json["shader"], "/System/Shaders/compute.comp");
        CHECK_EQ(json["compute"], true);

        auto deserializedOpt = FromJsonString<MaterialData>(json.dump());
        CHECK(deserializedOpt.has_value());

        auto deserialized = deserializedOpt.value();
        CHECK_EQ(deserialized.name, "compute_test");
        CHECK_EQ(deserialized.shader, "/System/Shaders/compute.comp");
        CHECK_EQ(deserialized.compute, true);
    }

    SUBCASE("Parse real skybox.material file format")
    {
        std::string skyboxMaterialJson = R"({
            "name": "skybox",
            "version": 0,
            "shader": "/System/Shaders/skybox.glsl",
            "offscreen": true,
            "depthCompareOp": "LESS_OR_EQUAL",
            "cullMode": "FRONT",
            "attachments": [
                {
                    "loadOperation": "LOAD",
                    "storeOperation": "STORE",
                    "dependency": 
                    {
                        "path": "/System/Materials/pbr.material",
                        "index": 0,
                        "hasDependency": true
                    }
                }
            ],
            "depthAttachment": 
            {
                "loadOperation": "LOAD",
                "storeOperation": "STORE",
                "dependency": 
                {
                    "path": "/System/Materials/pbr.material",
                    "index": 0,
                    "hasDependency": true
                }
            },
            "hasDepthAttachment": true
        })";

        auto materialDataOpt = FromJsonString<MaterialData>(skyboxMaterialJson);
        CHECK(materialDataOpt.has_value());

        auto materialData = materialDataOpt.value();
        CHECK_EQ(materialData.name, "skybox");
        CHECK_EQ(materialData.shader, "/System/Shaders/skybox.glsl");
        CHECK_EQ(materialData.version, 0);
        CHECK_EQ(materialData.offscreen, true);
        CHECK_EQ(materialData.depthCompareOp, "LESS_OR_EQUAL");
        CHECK_EQ(materialData.cullMode, "FRONT");
        CHECK_EQ(materialData.compute, false); // дефолтное значение
        CHECK_EQ(materialData.attachments.size(), 1);
        CHECK_EQ(materialData.hasDepthAttachment, true);

        const auto& attachment = materialData.attachments[0];
        CHECK_EQ(attachment.loadOperation, rhi::AttachmentLoadOperation::LOAD);
        CHECK_EQ(attachment.storeOperation, rhi::AttachmentStoreOperation::STORE);
        CHECK_EQ(attachment.dependency.hasDependency, true);
        CHECK_EQ(attachment.dependency.path, "/System/Materials/pbr.material");
        CHECK_EQ(attachment.dependency.index, 0);

        const auto& depthAttachment = materialData.depthAttachment;
        CHECK_EQ(depthAttachment.loadOperation, rhi::AttachmentLoadOperation::LOAD);
        CHECK_EQ(depthAttachment.storeOperation, rhi::AttachmentStoreOperation::STORE);
        CHECK_EQ(depthAttachment.dependency.hasDependency, true);
        CHECK_EQ(depthAttachment.dependency.path, "/System/Materials/pbr.material");
        CHECK_EQ(depthAttachment.dependency.index, 0);
    }

    SUBCASE("MaterialTextureSlot serialization")
    {
        MaterialTextureSlot textureSlot;
        textureSlot.texturePath = "/System/Textures/white.png";
        textureSlot.slot = 3;
        textureSlot.mipLevel = 0;

        auto json = ToJsonObject(textureSlot);

        CHECK_EQ(json["texturePath"], "/System/Textures/white.png");
        CHECK_EQ(json["slot"], 3);
        CHECK_EQ(json["mipLevel"], 0);

        auto deserializedOpt = FromJsonString<MaterialTextureSlot>(json.dump());
        CHECK(deserializedOpt.has_value());

        auto deserialized = deserializedOpt.value();
        CHECK_EQ(deserialized.texturePath.generic_string(), "/System/Textures/white.png");
        CHECK_EQ(deserialized.slot, 3);
        CHECK_EQ(deserialized.mipLevel, 0);
    }

    SUBCASE("MaterialData with texture slots")
    {
        MaterialData materialData;
        materialData.name = "pbr_material";
        materialData.shader = "/System/Shaders/pbr.glsl";
        materialData.version = 0;
        materialData.offscreen = true;
        materialData.depthCompareOp = "LESS";
        materialData.cullMode = "BACK";
        materialData.compute = false;
        
        // Добавляем текстурные слоты
        MaterialTextureSlot diffuseSlot;
        diffuseSlot.texturePath = "/System/Textures/white.png";
        diffuseSlot.slot = 3;
        diffuseSlot.mipLevel = 0;
        materialData.textureSlots.push_back(diffuseSlot);
        
        MaterialTextureSlot normalSlot;
        normalSlot.texturePath = "/System/Textures/normal_map.png";
        normalSlot.slot = 4;
        normalSlot.mipLevel = 0;
        materialData.textureSlots.push_back(normalSlot);
        
        MaterialTextureSlot brdfSlot;
        brdfSlot.texturePath = "/System/Textures/brdf_lut.tga";
        brdfSlot.slot = 10;
        brdfSlot.mipLevel = 0;
        materialData.textureSlots.push_back(brdfSlot);

        auto json = ToJsonObject(materialData);

        CHECK_EQ(json["name"], "pbr_material");
        CHECK_EQ(json["shader"], "/System/Shaders/pbr.glsl");
        CHECK(json["textureSlots"].is_array());
        CHECK_EQ(json["textureSlots"].size(), 3);
        
        // Проверяем текстурные слоты
        CHECK_EQ(json["textureSlots"][0]["texturePath"], "/System/Textures/white.png");
        CHECK_EQ(json["textureSlots"][0]["slot"], 3);
        CHECK_EQ(json["textureSlots"][1]["texturePath"], "/System/Textures/normal_map.png");
        CHECK_EQ(json["textureSlots"][1]["slot"], 4);
        CHECK_EQ(json["textureSlots"][2]["texturePath"], "/System/Textures/brdf_lut.tga");
        CHECK_EQ(json["textureSlots"][2]["slot"], 10);

        auto deserializedOpt = FromJsonString<MaterialData>(json.dump());
        CHECK(deserializedOpt.has_value());

        auto deserialized = deserializedOpt.value();
        CHECK_EQ(deserialized.name, "pbr_material");
        CHECK_EQ(deserialized.textureSlots.size(), 3);
        CHECK_EQ(deserialized.textureSlots[0].texturePath.generic_string(), "/System/Textures/white.png");
        CHECK_EQ(deserialized.textureSlots[0].slot, 3);
        CHECK_EQ(deserialized.textureSlots[1].texturePath.generic_string(), "/System/Textures/normal_map.png");
        CHECK_EQ(deserialized.textureSlots[1].slot, 4);
        CHECK_EQ(deserialized.textureSlots[2].texturePath.generic_string(), "/System/Textures/brdf_lut.tga");
        CHECK_EQ(deserialized.textureSlots[2].slot, 10);
    }

    SUBCASE("Parse material with texture slots from real file format")
    {
        std::string pbrMaterialJson = R"({
            "name": "pbr",
            "version": 0,
            "shader": "/System/Shaders/pbr.glsl",
            "offscreen": true,
            "depthCompareOp": "LESS",
            "cullMode": "BACK",
            "compute": false,
            "attachments": [
                {
                    "loadOperation": "CLEAR",
                    "storeOperation": "STORE",
                    "dependency": {
                        "path": "",
                        "index": 0,
                        "hasDependency": false
                    }
                }
            ],
            "depthAttachment": {
                "loadOperation": "CLEAR",
                "storeOperation": "STORE",
                "dependency": {
                    "path": "",
                    "index": 0,
                    "hasDependency": false
                }
            },
            "hasDepthAttachment": true,
            "textureSlots": [
                {
                    "texturePath": "/System/Textures/white.png",
                    "slot": 3,
                    "mipLevel": 0
                },
                {
                    "texturePath": "/System/Textures/normal_map.png",
                    "slot": 4,
                    "mipLevel": 0
                },
                {
                    "texturePath": "/System/Textures/brdf_lut.tga",
                    "slot": 10,
                    "mipLevel": 0
                }
            ]
        })";

        auto materialDataOpt = FromJsonString<MaterialData>(pbrMaterialJson);
        CHECK(materialDataOpt.has_value());

        auto materialData = materialDataOpt.value();
        CHECK_EQ(materialData.name, "pbr");
        CHECK_EQ(materialData.shader.generic_string(), "/System/Shaders/pbr.glsl");
        CHECK_EQ(materialData.compute, false);
        CHECK_EQ(materialData.textureSlots.size(), 3);

        // Проверяем текстурные слоты
        CHECK_EQ(materialData.textureSlots[0].texturePath.generic_string(), "/System/Textures/white.png");
        CHECK_EQ(materialData.textureSlots[0].slot, 3);
        CHECK_EQ(materialData.textureSlots[0].mipLevel, 0);

        CHECK_EQ(materialData.textureSlots[1].texturePath.generic_string(), "/System/Textures/normal_map.png");
        CHECK_EQ(materialData.textureSlots[1].slot, 4);
        CHECK_EQ(materialData.textureSlots[1].mipLevel, 0);

        CHECK_EQ(materialData.textureSlots[2].texturePath.generic_string(), "/System/Textures/brdf_lut.tga");
        CHECK_EQ(materialData.textureSlots[2].slot, 10);
        CHECK_EQ(materialData.textureSlots[2].mipLevel, 0);
    }
} 