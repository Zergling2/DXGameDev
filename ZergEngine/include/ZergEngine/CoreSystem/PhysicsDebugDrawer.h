#pragma once

#include <ZergEngine\CoreSystem\InputLayout.h>
#include <bullet3\LinearMath\btIDebugDraw.h>
#include <vector>

namespace ze
{
	using DebugLineVertexFormat = VFPositionColor;

	class PhysicsDebugDrawer : public btIDebugDraw
	{
	public:
        PhysicsDebugDrawer();
		virtual ~PhysicsDebugDrawer() = default;

		bool Init(ID3D11Device* pDevice);
		void Release();

		void UpdateResources(ID3D11DeviceContext* pDeviceContext);
		void ClearDebugDrawerCache();

		ID3D11Buffer* GetDebugLineVertexBuffer() const { return m_cpDebugLineVertexBuffer.Get(); }
		ID3D11RasterizerState* GetRSDebugLineDrawing() const { return m_cpRSDebugLineDrawing.Get(); }
		UINT GetDebugLineVertexCountToDraw() const;

        // btIDebugDraw 인터페이스
        virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

        virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, 
			int lifeTime, const btVector3& color) override;

        virtual void reportErrorWarning(const char* warningString) override;

		virtual void draw3dText(const btVector3& location, const char* textString) override;

        virtual void setDebugMode(int debugMode) override;

        virtual int getDebugMode() const override;
	private:
		std::vector<DebugLineVertexFormat> m_debugLineVertices;
		int m_debugMode;
		ComPtr<ID3D11Buffer> m_cpDebugLineVertexBuffer;
		ComPtr<ID3D11RasterizerState> m_cpRSDebugLineDrawing;
	};
}
