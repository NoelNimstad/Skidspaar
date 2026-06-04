#include <catch2/catch_amalgamated.hpp>

#include "../source/spaar.hpp"
#include "TESTING_UTILITIES.hpp"

using namespace Spår::Utility::Types;

TEST_CASE("Vectors")
{
	SECTION("Special vectors")
	{
		// Zero vector
		CHECK(Vector3() == Vector3(0, 0, 0));	
		
		// i, j, k
		CHECK(Vector3::i() == Vector3(1, 0, 0));	
		CHECK(Vector3::j() == Vector3(0, 1, 0));	
		CHECK(Vector3::k() == Vector3(0, 0, 1));
		// Combinations
		CHECK(Vector2::i() * 3 - Vector2::j() * 2 == Vector2(3, -2));
		CHECK(3 * Vector2::i() - 2 * Vector2::j() == Vector2(3, -2));
	};

	SECTION("Magnitude caching")
	{
		Vector3 v(0, 0, 0);

		SECTION("Safe operations")
		{
			CHECK(v.magnitude() == 0);
	
			v.x(1);
	
			CHECK(v.magnitude() == 1);

			v.x(0);

			CHECK(v.magnitude() == 0);
		}

		SECTION("Unsafe operations")
		{
			v[0] = 1;
	
			CHECK_FALSE(v.magnitude() == 1);
	
			v.unsafeAt(0) = 1;
	
			CHECK_FALSE(v.magnitude() == 1);
	
			v.markMagnitudeDirty();
	
			CHECK(v.magnitude() == 1);
		}
	};

	SECTION("Vector operations")
	{
		// Parallel
		CHECK( (Vector3(1, 2, 3) || Vector3(-2, -4, -6)) );
		CHECK_FALSE( (Vector3(1, 2, 3) || Vector3(-1, -4, -6)) );
		CHECK_FALSE( (Vector3(1, 2, 3) || Vector3(-2, -3, -6)) );
		CHECK_FALSE( (Vector3(1, 2, 3) || Vector3(-2, -4, -5)) );

		// Cross product
		CHECK(Vector3::i().cross(Vector3::j()) == Vector3::k());
		CHECK(Vector3::i().cross(Vector3::k()) == -Vector3::j());
		CHECK(Vector3::j().cross(Vector3::k()) == Vector3::i());

		// Angle
		CHECK(Vector2_d::i().acuteAngleTo(Vector2_d::i()) == 0);
		CHECK(Vector2_d::i().acuteAngleTo(Vector2_d::j()) == M_PI_2);
		CHECK(WITHIN_ε<float64_t>(Vector2_d::i().acuteAngleTo(Vector2_d(1, 1)), M_PI_4));
		CHECK(WITHIN_ε<float64_t>(Vector2_d::fromPolar(1, 2 * M_PI).angle(), 0));
	};
}