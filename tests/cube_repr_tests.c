#include "../include/libcube.h"
#include "../src/utils.h"
#include "../src/_cube.h"
#include "criterion/criterion.h"
#include "criterion/redirect.h"
#include <stdint.h>
#include <stdio.h>

// suites

TestSuite(cube_repr);

// tests

Test(cube_repr, extract_eo){
  uint16_t edge1 = 0b1011001;
  uint16_t edge2 = 0b1011000;
  uint16_t edge3 = 0b1011011;
  uint16_t edge4 = 0b1011101;

  cr_assert_eq(extract_edge_orien(edge1, FB), 0);
  cr_assert_eq(extract_edge_orien(edge1, LR), 0);
  cr_assert_eq(extract_edge_orien(edge1, UD), 1);
  cr_assert_eq(extract_edge_orien(edge2, FB), 0);
  cr_assert_eq(extract_edge_orien(edge2, LR), 0);
  cr_assert_eq(extract_edge_orien(edge2, UD), 0);
  cr_assert_eq(extract_edge_orien(edge3, FB), 0);
  cr_assert_eq(extract_edge_orien(edge3, LR), 1);
  cr_assert_eq(extract_edge_orien(edge3, UD), 1);
  cr_assert_eq(extract_edge_orien(edge4, FB), 1);
  cr_assert_eq(extract_edge_orien(edge4, LR), 0);
  cr_assert_eq(extract_edge_orien(edge4, UD), 1);
}

Test(cube_repr, extract_co){
  uint16_t corner1 = 0b010101111;
  uint16_t corner2 = 0b000011111;
  uint16_t corner3 = 0b110000111;
  uint16_t corner4 = 0b001100111;

  cr_assert_eq(extract_corner_orien(corner1, FB), 0b01);
  cr_assert_eq(extract_corner_orien(corner1, LR), 0b01);
  cr_assert_eq(extract_corner_orien(corner1, UD), 0b01);
  cr_assert_eq(extract_corner_orien(corner2, FB), 0b11);
  cr_assert_eq(extract_corner_orien(corner2, LR), 0b00);
  cr_assert_eq(extract_corner_orien(corner2, UD), 0b00);
  cr_assert_eq(extract_corner_orien(corner3, FB), 0b00);
  cr_assert_eq(extract_corner_orien(corner3, LR), 0b00);
  cr_assert_eq(extract_corner_orien(corner3, UD), 0b11);
  cr_assert_eq(extract_corner_orien(corner4, FB), 0b00);
  cr_assert_eq(extract_corner_orien(corner4, LR), 0b11);
  cr_assert_eq(extract_corner_orien(corner4, UD), 0b00);
}

Test(cube_repr, extract_ep){
  uint16_t edge1 = 0b1011001;
  uint16_t edge2 = 0b1001000;
  uint16_t edge3 = 0b1111011;
  uint16_t edge4 = 0b0000101;

  cr_assert_eq(extract_edge_perm(edge1), 0b1011);
  cr_assert_eq(extract_edge_perm(edge2), 0b1001);
  cr_assert_eq(extract_edge_perm(edge3), 0b1111);
  cr_assert_eq(extract_edge_perm(edge4), 0b0000);
}

Test(cube_repr, extract_cp){
  uint16_t corner1 = 0b010101111;
  uint16_t corner2 = 0b000011101;
  uint16_t corner3 = 0b110000110;
  uint16_t corner4 = 0b001100000;

  cr_assert_eq(extract_corner_perm(corner1), 0b111);
  cr_assert_eq(extract_corner_perm(corner2), 0b101);
  cr_assert_eq(extract_corner_perm(corner3), 0b110);
  cr_assert_eq(extract_corner_perm(corner4), 0b000);
}

Test(cube_repr, update_eo){
  uint16_t edge1 = 0b0000000;
  uint16_t edge2 = 0b1111000;

  update_edge_orien(&edge1, 1, 1, 1);
  cr_assert_eq(edge1, 0b0000111);

  update_edge_orien(&edge1, 1, 0, 0);
  cr_assert_eq(edge1, 0b0000100);
  
  update_edge_orien(&edge1, 0, 1, 0);
  cr_assert_eq(edge1, 0b0000010);

  update_edge_orien(&edge1, 0, 0, 1);
  cr_assert_eq(edge1, 0b0000001);

  update_edge_orien(&edge2, 1, 1, 1);
  cr_assert_eq(edge2, 0b1111111);

  update_edge_orien(&edge2, 1, 0, 0);
  cr_assert_eq(edge2, 0b1111100);
  
  update_edge_orien(&edge2, 0, 1, 0);
  cr_assert_eq(edge2, 0b1111010);

  update_edge_orien(&edge2, 0, 0, 1);
  cr_assert_eq(edge2, 0b1111001);
}

Test(cube_repr, update_ep){
  uint16_t edge1 = 0b0000111;
  uint16_t edge2 = 0b1111001;

  update_edge_perm(&edge1, 1);
  cr_assert_eq(edge1, 0b0001111);
  update_edge_perm(&edge1, 2);
  cr_assert_eq(edge1, 0b0010111);
  update_edge_perm(&edge1, 3);
  cr_assert_eq(edge1, 0b0011111);
  update_edge_perm(&edge1, 4);
  cr_assert_eq(edge1, 0b0100111);

  update_edge_perm(&edge2, 1);
  cr_assert_eq(edge2, 0b0001001);
}

Test(cube_repr, update_co){
  uint16_t corner1 = 0b000000000;
  uint16_t corner2 = 0b000000111;

  update_corner_orien(&corner1, 1, 1, 1);
  cr_assert_eq(corner1, 0b010101000);

  update_corner_orien(&corner1, 0, 0, 1);
  cr_assert_eq(corner1, 0b010000000);
  
  update_corner_orien(&corner1, 0, 1, 0);
  cr_assert_eq(corner1, 0b000100000);

  update_corner_orien(&corner1, 1, 0, 0);
  cr_assert_eq(corner1, 0b000001000);

  update_corner_orien(&corner2, 1, 1, 1);
  cr_assert_eq(corner2, 0b010101111);

  update_corner_orien(&corner2, 0, 0, 1);
  cr_assert_eq(corner2, 0b010000111);
  
  update_corner_orien(&corner2, 0, 1, 0);
  cr_assert_eq(corner2, 0b000100111);

  update_corner_orien(&corner2, 1, 0, 0);
  cr_assert_eq(corner2, 0b000001111);

  update_corner_orien(&corner2, 1, 2, 2);
  cr_assert_eq(corner2, 0b101001111);
}

Test(cube_repr, update_cp){
  uint16_t corner1 = 0b111111000;
  uint16_t corner2 = 0b000000000;

  update_corner_perm(&corner1, 1);
  cr_assert_eq(corner1, 0b111111001);
  update_corner_perm(&corner1, 2);
  cr_assert_eq(corner1, 0b111111010);
  update_corner_perm(&corner1, 3);
  cr_assert_eq(corner1, 0b111111011);
  update_corner_perm(&corner1, 4);
  cr_assert_eq(corner1, 0b111111100);

  update_corner_perm(&corner2, 7);
  cr_assert_eq(corner2, 0b000000111);
}

Test(cube_repr, build_corner){
  cr_assert_eq(build_corner(0b100, 0b10, 0, 0), 0b10100);
  cr_assert_eq(build_corner(0b100, 0b01, 0, 0), 0b01100);
  cr_assert_eq(build_corner(0b111, 0b10, 0, 0), 0b10111);
  cr_assert_eq(build_corner(0b001, 0b10, 0, 0), 0b10001);
  cr_assert_eq(build_corner(0b000, 0b00, 0, 0), 0b00000);

  cr_assert_eq(build_corner(0b100, 0b10, 0, 0b10), 0b100010100);
  cr_assert_eq(build_corner(0b100, 0b01, 0, 0b10), 0b100001100);
  cr_assert_eq(build_corner(0b111, 0b10, 0b01, 0b10), 0b100110111);
  cr_assert_eq(build_corner(0b001, 0b10, 0, 0b01), 0b010010001);
  cr_assert_eq(build_corner(0b000, 0b00, 0, 0b01), 0b010000000);
}

Test(cube_repr, build_edge){
  cr_assert_eq(build_edge(0b0100, 0b1, 0, 0), 0b0100100);
  cr_assert_eq(build_edge(0b0100, 0b0, 0, 0), 0b0100000);
  cr_assert_eq(build_edge(0b0111, 0b1, 0, 0), 0b0111100);
  cr_assert_eq(build_edge(0b0001, 0b1, 0, 0), 0b0001100);
  cr_assert_eq(build_edge(0b0000, 0b0, 0, 0), 0b0000000);
}