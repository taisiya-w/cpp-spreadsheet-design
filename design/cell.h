#pragma once

#include "common.h"
#include "formula.h"

#include <memory>
#include <optional>
#include <unordered_set>
#include <vector>

class SheetInterface;

class Cell : public CellInterface {
public:
    explicit Cell(SheetInterface& sheet);
    ~Cell();

    void Set(std::string text);

    void Clear();

    Value GetValue() const override;

    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;

    void AddDependent(Cell* cell);

    void RemoveDependent(Cell* cell);

    const std::unordered_set<Cell*>& GetDependents() const;

    void InvalidateCache();

    bool IsCacheValid() const;

private:

    class Impl {
    public:
        virtual ~Impl() = default;
        virtual Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
        virtual std::vector<Position> GetReferencedCells() const = 0;
    };

    class EmptyImpl : public Impl {
    public:
        Value GetValue() const override;
        std::string GetText() const override;
        std::vector<Position> GetReferencedCells() const override;
    };

    class TextImpl : public Impl {
    public:
        explicit TextImpl(std::string text);
        Value GetValue() const override;
        std::string GetText() const override;
        std::vector<Position> GetReferencedCells() const override;
    private:
        std::string text_;
    };

    class FormulaImpl : public Impl {
    public:
        explicit FormulaImpl(std::string expression, SheetInterface& sheet);
        Value GetValue() const override;
        std::string GetText() const override;
        std::vector<Position> GetReferencedCells() const override;
    private:
        std::unique_ptr<FormulaInterface> formula_;
        SheetInterface& sheet_;  // не владеет
    };

    std::unique_ptr<Impl> impl_;

    SheetInterface& sheet_;

    mutable std::optional<Value> cache_;

    std::unordered_set<Cell*> dependents_;
};