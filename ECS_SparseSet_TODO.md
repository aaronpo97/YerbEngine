# ECS Sparse Set Refactor TODO

**Goal:**
Refactor the entity-component system to use sparse sets/pools for each component type, enabling efficient access and iteration over entities with specific components.

---

## Tasks

- [ ] Analyze current entity-component storage and access patterns
- [ ] Design per-component pools (e.g., `std::unordered_map<EntityId, CTransform>`)
- [ ] Refactor entity objects to store only ID and component signature/bitmask
- [ ] Implement component pools for each type (CTransform, CShape, etc.)
- [ ] Update component add/remove logic to operate on pools
- [ ] Update systems to iterate over component pools instead of all entities
- [ ] Ensure efficient lookup and iteration for systems
- [ ] Migrate existing code to use new pattern
- [ ] Test for correctness and performance improvements
- [ ] Document new ECS architecture and usage

---

**Description:**
A sparse set (or pool) stores each component type in its own container, mapping entity IDs to component instances. Entities only store their ID and a signature/bitmask of their components. Adding/removing components means inserting/removing from the relevant pool. Systems iterate over pools for efficient processing. This improves cache locality, speeds up iteration, and simplifies component management.
