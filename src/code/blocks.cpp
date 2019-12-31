#include "../headers/blocks.hpp"
#include "../headers/tree.hpp"

CodeBlock::CodeBlock(std::vector<std::shared_ptr<CodeBlock>> nestedBlocks, BlockType blockType) : type(blockType), nested(nestedBlocks) {

    for(auto block: nested){
        block.get() -> parent = std::shared_ptr<CodeBlock>(this);
    }
}

CodeBlock::CodeBlock( std::shared_ptr<CodeBlock> nestedBlock, BlockType blockType): type(blockType) {
    this -> nested.push_back(nestedBlock);
    nestedBlock.get() -> parent = std::shared_ptr<CodeBlock>(this);
}

void CodeBlock::setForLoop(std::shared_ptr<Variable> iterator){
    this->forLoop = true;
    this->iterator = iterator;
}

std::shared_ptr<CodeBlock> CodeBlock::getParent(){
    return this -> parent;
}
BlockType CodeBlock::getType(){
    return this -> type;
}
std::map<std::string, std::shared_ptr<Variable>> CodeBlock::getLocalVariables(){
    std::shared_ptr<CodeBlock> curr(this);
    std::map<std::string, std::shared_ptr<Variable>> localVariables;
    while(curr.get()->parent != nullptr){
        if(curr.get() -> forLoop){
            localVariables[curr.get()->iterator.get()->getName()] = curr.get()->iterator;
        }
    }

    return localVariables;
    
}
std::vector<std::shared_ptr<CodeBlock>> CodeBlock::getNested(){
    return this -> nested;
}
std::vector<std::shared_ptr<Asm>> CodeBlock::getAssembler(){
    return this -> assembler;
}





